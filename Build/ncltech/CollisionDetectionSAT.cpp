#include "CollisionDetectionSAT.h"
#include <nclgl\NCLDebug.h>
#include "GeometryUtils.h"

using namespace GeometryUtils;

CollisionDetectionSAT::CollisionDetectionSAT()
{
}

void CollisionDetectionSAT::BeginNewPair(
	PhysicsNode* obj1,
	PhysicsNode* obj2,
	CollisionShape* shape1,
	CollisionShape* shape2)
{
	possibleColAxes.clear();

	pnodeA = obj1;
	pnodeB = obj2;
	cshapeA = obj1->GetNarrowCollisionShape();
	cshapeB = obj2->GetNarrowCollisionShape();

	areColliding = false;
}



bool CollisionDetectionSAT::AreColliding(CollisionData* out_coldata)
{
	if (!cshapeA || !cshapeB) {
		return false;
	}

	areColliding = false;
	possibleColAxes.clear();

	FindAllPossibleCollisionAxes();

	// SAT: If a single axis is found along which the two objects are not colliding then they are not colliding
	// Check each possible axis and either return false of return the best possible axis (one with least penetration)
	bestColData._penetration = -FLT_MAX;

	for (const Vector3& axis : possibleColAxes) {
		CollisionData currentCollision;
		// If find any axis where objects are not colliding then cannot possibly intersect
		if (!CheckCollisionAxis(axis, currentCollision)) {
			return false;
		}

		if (currentCollision._penetration >= bestColData._penetration) {
			bestColData = currentCollision;
		}

	}
	
	if (out_coldata) {
		*out_coldata = bestColData;
	}
	areColliding = true;
	return true;
}

void CollisionDetectionSAT::FindAllPossibleCollisionAxes() {
	// Default axes - depend on specific shape
	std::vector<Vector3> defaultAxesA, defaultAxesB;
	cshapeA->GetCollisionAxes(pnodeB, defaultAxesA);
	cshapeB->GetCollisionAxes(pnodeA, defaultAxesB);
	
	for (const Vector3& axis : defaultAxesA) {
		AddPossibleCollisionAxis(axis);
	}
	for (const Vector3& axis : defaultAxesB) {
		AddPossibleCollisionAxis(axis);
	}

	 //Edge-Edge Cases
	for (const Vector3& norm1 : defaultAxesA) {
		for (const Vector3& norm2 : defaultAxesB) {
			AddPossibleCollisionAxis(Vector3::Cross(norm1, norm2).Normalise());
		}
	}
}

bool CollisionDetectionSAT::CheckCollisionAxis(const Vector3& axis, CollisionData& out_coldata)
{
	//Overlap Test
	// Points go: 
	//          +-------------+
	//  +-------|-----+   2   |
	//  |   1   |     |       |
	//  |       +-----|-------+
	//  +-------------+
	//  A ------C --- B ----- D 
	//
	//	IF	 A < C AND B > C (Overlap in order object 1 -> object 2)
	//	IF	 C < A AND D > A (Overlap in order object 2 -> object 1)

	Vector3 minA, maxA, minB, maxB;

	cshapeA->GetMinMaxVertexOnAxis(axis, minA, maxA);
	cshapeB->GetMinMaxVertexOnAxis(axis, minB, maxB);

	float A = Vector3::Dot(axis, minA);
	float B = Vector3::Dot(axis, maxA);
	float C = Vector3::Dot(axis, minB);
	float D = Vector3::Dot(axis, maxB);

	// Overlap test : Object 1 before Object 2
	if (A <= C && B >= C) {
		out_coldata._normal = axis;
		out_coldata._penetration = C - B;
		out_coldata._pointOnPlane = maxA + out_coldata._normal * out_coldata._penetration;

		return true;
	}
	// Overlap test : Object 2 before Object 1
	if (C <= A && D >= A) {
		out_coldata._normal = axis;
		out_coldata._penetration = C - B;
		out_coldata._pointOnPlane = maxA + out_coldata._normal * out_coldata._penetration;

		return true;
	}
	// No overlap on this axis found
	return false;
}






void CollisionDetectionSAT::GenContactPoints(Manifold* out_manifold)
{
	if (!out_manifold || !areColliding || bestColData._penetration >= 0.0f) {
		return;
	}

	// Get face data for the two shapes based on the collision normal
	std::list<Vector3> polygonA, polygonB;			// Store face information
	Vector3 normalA, normalB;						// Store normal information
	std::vector<Plane> adjPlanesA, adjPlanesB;		// Store plane information(??)

	cshapeA->GetIncidentReferencePolygon(bestColData._normal, polygonA, normalA, adjPlanesA);
	cshapeB->GetIncidentReferencePolygon(-bestColData._normal, polygonB, normalB, adjPlanesB);

	if (polygonA.empty() || polygonB.empty()) {
		return;
	}// If either shape returned a a single point then it must be on a curve and can already access its contact point
	else if (polygonA.size() == 1) {
		out_manifold->AddContact(
			polygonA.front(), // Polygon2 <-- Polygon1 (??)
			polygonA.front() + bestColData._normal *
			bestColData._penetration,
			bestColData._normal,
			bestColData._penetration);
	}
	else if (polygonB.size() == 1) {
		out_manifold->AddContact(
			polygonB.front() + bestColData._normal *
			bestColData._penetration, // Polygon2 <-- Polygon1 (??)
			polygonB.front(),
			bestColData._normal,
			bestColData._penetration);
	}
	else { // Otherwise clip the incident plane to inside the reference plane using surrounding face planes

		// Do we need to flip the incident and reference faces around for clipping?
		bool isFlipped = fabs(Vector3::Dot(bestColData._normal, normalA)) < fabs(Vector3::Dot(bestColData._normal, normalB));

		if (isFlipped) {
			std::swap(polygonA, polygonB);
			std::swap(normalA, normalB);
			std::swap(adjPlanesA, adjPlanesB);
		}

		// Clip the incident face to the adjacent edges of the reference faces
		if (!adjPlanesA.empty()) {
			SutherlandHodgmanClipping(polygonB, adjPlanesA.size(), &adjPlanesA[0], &polygonB, false);
		}

		// Clip and remove any contact points above the reference face
		Plane refPlane = Plane(-normalA, -Vector3::Dot(-normalA, polygonA.front()));
		SutherlandHodgmanClipping(polygonB, 1, &refPlane, &polygonB, true);

		// Generate manifold from the remaining valid contact points
		for (const Vector3& point : polygonB) {
			// Distance to the reference plane
			Vector3 pointDiff = point - GetClosestPointPolygon(point, polygonA);
			float contactPenetration = Vector3::Dot(pointDiff, bestColData._normal);

			// Contact data
			Vector3 globalOnA;
			Vector3 globalOnB;

			// If we flipped incident and reference planes need to flip it back before sending it to the manifold
			// e.g. turn it from objectB->objectA into objectA->objectB

			if (isFlipped) {
				contactPenetration = -contactPenetration;
				globalOnA = point + bestColData._normal * contactPenetration;
				globalOnB = point;
			}
			else {
				globalOnA = point;
				globalOnB = point - bestColData._normal * contactPenetration;
			}

			// Sanity check that the contact point is an actual point of contact and not just a clipping bug
			if (contactPenetration < 0.0f) {
				out_manifold->AddContact(globalOnA, globalOnB, bestColData._normal, contactPenetration);
			}
		}

		
	}
	

}




bool CollisionDetectionSAT::AddPossibleCollisionAxis(Vector3 axis)
{
	const float epsilon = 1e-6f;

	//is axis 0,0,0??
	if (Vector3::Dot(axis, axis) < epsilon)
		return false;

	axis.Normalise();

	for (const Vector3& p_axis : possibleColAxes)
	{
		//Is axis very close to the same as a previous axis already in the list of axes??
		if (Vector3::Dot(axis, p_axis) >= 1.0f - epsilon)
			return false;
	}

	possibleColAxes.push_back(axis);
	return true;
}


