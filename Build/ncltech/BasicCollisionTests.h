// Contains some simple tests useful in broad phase collisions when collision data 
// is not needed

#pragma once
#include "SphereCollisionShape.h"
#include "CuboidCollisionShape.h"
#include "CompositeCollisionShape.h"
#include "BoundingCube.h"
#include "PhysicsNode.h"


bool AreColliding(SphereCollisionShape* shapeA, SphereCollisionShape* shapeB) {
	if (!shapeA || !shapeB) {
		return false;
	}
	float minSquareSeperation = (shapeA->GetRadius() + shapeB->GetRadius()) * (shapeA->GetRadius() + shapeB->GetRadius());
	float squareSeperation = (shapeA->Parent()->GetPosition() - shapeB->Parent()->GetPosition()).LengthSq();
	return squareSeperation < minSquareSeperation;
}

bool AreColliding(SphereCollisionShape* shapeA, CuboidCollisionShape* shapeB) {
	if (!shapeA || !shapeB) {
		return false;
	}
	return false;
}


bool AreColliding(SphereCollisionShape* shapeA, CompositeCollisionShape* shapeB) {
	if (!shapeA || !shapeB) {
		return false;
	}
	return false;
}

