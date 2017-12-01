// Contains some simple tests useful in broad phase collisions when collision data 
// is not needed

#pragma once
#include "SphereCollisionShape.h"
#include "PhysicsNode.h"

bool AreColliding(SphereCollisionShape* shapeA, SphereCollisionShape* shapeB) {
	if (!shapeA || !shapeB) {
		return false;
	}
	float minSquareSeperation = (shapeA->GetRadius() + shapeB->GetRadius()) * (shapeA->GetRadius() + shapeB->GetRadius());
	float squareSeperation = (shapeA->Parent()->GetPosition() - shapeB->Parent()->GetPosition()).LengthSq();
	return squareSeperation < minSquareSeperation;
}
