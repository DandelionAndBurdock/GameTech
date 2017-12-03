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

	// Draws this collision shape to the debug renderer
	void DebugDraw() const override;

	float GetSize() override;
	//<----- USED BY COLLISION DETECTION ----->
	// Get all collision axes between the current shape and the given
	// object to be tested. The axes returned are just for the shape in question
	// the other shape will also have it's own axes to test.
	 void GetCollisionAxes(
		const PhysicsNode* otherObject,
		std::vector<Vector3>& out_axes) const override;

	//Returns closest point on the collision shape to the given point
	virtual Vector3 GetClosestPoint(const Vector3& point) const override;

	// Get the min/max vertices along a given axis
	void GetMinMaxVertexOnAxis(
		const Vector3& axis,
		Vector3& out_min,
		Vector3& out_max) const override;


	// Get all data needed to build manifold
	//	- Computes the face that is closest to parallel to that of the given axis,
	//    returning the face (as a list of vertices), face normal and the planes
	//    of all adjacent faces in order to clip against.
	virtual void GetIncidentReferencePolygon(
		const Vector3& axis,
		std::list<Vector3>& out_face,
		Vector3& out_normal,
		std::vector<Plane>& out_adjacent_planes) const override;
private:
	// Shapes making up this object
	std::vector<CollisionShape*> shapes;

};


