#include "CompositeCollisionShape.h"



CompositeCollisionShape::CompositeCollisionShape()
{
}


CompositeCollisionShape::~CompositeCollisionShape()
{
}




Matrix3 CompositeCollisionShape::BuildInverseInertia(float invMass) const {
	return Matrix3();
}

// Draws this collision shape to the debug renderer
void CompositeCollisionShape::DebugDraw() const {
	for (const auto& shape : shapes) {
		shape->DebugDraw();
	}
}

float CompositeCollisionShape::GetSize() {
	float maxSize = 0.0f;
	for (const auto& shape : shapes) { // This will almost certainly overestimate the size
		maxSize += shape->GetSize();
	}
	return maxSize;
}


void CompositeCollisionShape::GetCollisionAxes(
	const PhysicsNode* otherObject,
	std::vector<Vector3>& out_axes) const {
	for (const auto& shape : shapes) { 
		shape->GetCollisionAxes(otherObject, out_axes);
	}
}


Vector3 CompositeCollisionShape::GetClosestPoint(const Vector3& point) const {
	float closestPointDistance = FLT_MAX;
	Vector3 closestPoint;
	for (const auto& shape : shapes) {
		Vector3 currentClosest = shape->GetClosestPoint(point);
		if (Vector3::DistanceSq(currentClosest, point) < closestPointDistance) {
			closestPoint = currentClosest;
		}
	}
	return closestPoint;
}


void CompositeCollisionShape::GetMinMaxVertexOnAxis(
	const Vector3& axis,
	Vector3& out_min,
	Vector3& out_max) const {

}

void CompositeCollisionShape::GetIncidentReferencePolygon(
	const Vector3& axis,
	std::list<Vector3>& out_face,
	Vector3& out_normal,
	std::vector<Plane>& out_adjacent_planes) const {

}