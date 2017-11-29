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
	cshapeA = obj1->GetCollisionShape();
	cshapeB = obj2->GetCollisionShape();

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

	// Edge-Edge Cases
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
 /* TUTORIAL 5 CODE */
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


