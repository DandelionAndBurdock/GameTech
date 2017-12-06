#include "CompositeCollisionShape.h"



CompositeCollisionShape::CompositeCollisionShape()
{
}


CompositeCollisionShape::~CompositeCollisionShape()
{
}


// Double dispatch
bool CompositeCollisionShape::IsColliding(CollisionShape* shape) {
	return false;
}

bool CompositeCollisionShape::IsCollidingWith(SphereCollisionShape* shape) {
	return false;
}

Matrix3 CompositeCollisionShape::BuildInverseInertia(float invMass) const {
	return Matrix3();
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

// Draws this collision shape to the debug renderer
void CompositeCollisionShape::DebugDraw(Vector3 offset) const {
	for (int i = 0; i < shapes.size(); ++i) {
		shapes[i]->DebugDraw(offsets[i] + offset);
	}
}

float CompositeCollisionShape::GetSize() {
	float maxSize = 0.0f;
	for (const auto& shape : shapes) { // This will almost certainly overestimate the size
		maxSize += shape->GetSize();
	}
	return maxSize;
}

void CompositeCollisionShape::AddShape(CollisionShape* shape, Vector3 offset) {
	shapes.push_back(shape);
	offsets.push_back(offset);
}

void CompositeCollisionShape::SetParent(PhysicsNode* node) {
	m_Parent = node;
	for (auto& shape : shapes) {
		shape->SetParent(node);
	}
}