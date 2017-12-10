#pragma once

#include "CollisionShape.h"
#include "CuboidCollisionShape.h"
#include "SphereCollisionShape.h"


class CompositeCollisionShape : public CollisionShape
{
public:
	CompositeCollisionShape();
	~CompositeCollisionShape();
	Matrix3 BuildInverseInertia(float invMass) const override;
	Matrix3 BuildInertia(float invMass) const override;
	Vector3 GetClosestPoint(const Vector3& point) const;
	void SetParent(PhysicsNode* node) override;
	void AddShape(CollisionShape* shape, Vector3 offset);

	// Draws this collision shape to the debug renderer
	void DebugDraw(Vector3 offset = Vector3(0.0f)) const override;

	float GetSize() override;

	// Double dispatch
	bool IsColliding(CollisionShape* shape) override;
	bool IsCollidingWith(SphereCollisionShape* shape) override;
	bool IsCollidingWith(CuboidCollisionShape* shape) override;
	bool IsCollidingWith(CompositeCollisionShape* shape) override;
private:
	// Shapes making up this object
	std::vector<CollisionShape*> shapes;
	// Offset of each shape from node position 
	std::vector<Vector3> offsets;

};


