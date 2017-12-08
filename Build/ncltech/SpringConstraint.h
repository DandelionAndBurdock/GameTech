#pragma once
/******************************************************************************
Class: DistanceConstraint
Implements:
Author:
Pieran Marris      <p.marris@newcastle.ac.uk> and YOU!
Description:

Bassed on the simple derivation at
https://www.gamedev.net/articles/programming/math-and-physics/towards-a-simpler-stiffer-and-more-stable-spring-r3227/

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DistanceConstraint.h"
#include "PhysicsEngine.h"
#include <nclgl\NCLDebug.h>

class SpringConstraint : public DistanceConstraint
{
public:
	SpringConstraint(PhysicsNode* obj1, PhysicsNode* obj2,
		const Vector3& globalOnA, const Vector3& globalOnB)
		: DistanceConstraint(obj1, obj2, globalOnA, globalOnB)
	{
	}

	//Solves the constraint and applies a velocity impulse to the two
	// objects in order to satisfy the constraint.
	virtual void ApplyImpulse(float dt) override
	{
		// Vector from centre of object to the contact point
		Vector3 r1 = pnodeA->GetOrientation().ToMatrix3() * relPosA;
		Vector3 r2 = pnodeB->GetOrientation().ToMatrix3() * relPosB;

		// Contact point position in world space
		Vector3 globalOnA = r1 + pnodeA->GetPosition();
		Vector3 globalOnB = r2 + pnodeB->GetPosition();

		// Vector between two contact points
		Vector3 ab = globalOnB - globalOnA;

		// Unit vector from a to b
		Vector3 abn = ab;
		abn.Normalise();

		//
		Vector3 v0 = pnodeA->GetLinearVelocity() +
			Vector3::Cross(pnodeA->GetAngularVelocity(), r1);

		Vector3 v1 = pnodeB->GetLinearVelocity() +
			Vector3::Cross(pnodeB->GetAngularVelocity(), r2);

		// Relative velocity in the constraint direction
		float constraintVel = Vector3::Dot(v0 - v1, abn);


		// Combined inverse mass of a and b
		float invConstraintMass = pnodeA->GetInverseMass() +
			pnodeB->GetInverseMass();


		// Reduced mass of the system
		float reducedMass = 1.0f / invConstraintMass;

		float invConstraintInertia =
			Vector3::Dot(abn,
				Vector3::Cross(pnodeA->GetInverseInertia() *
					Vector3::Cross(r1, abn), r1) +
				Vector3::Cross(pnodeB->GetInverseInertia() *
					Vector3::Cross(r2, abn), r2));

		float totalConstraintMass = invConstraintInertia + invConstraintMass;

		if (totalConstraintMass > 0.0f) {
			// Baumgarte Offset
			// Adds energy to the system to counter slight solving errors that accumulate
			// over time ("Constraint Drift")

			float b = 0.0f;

			float distanceOffset = ab.Length() - targetLength;
			// This value changes based on physics objects, timestep, number of constraints etc. 
			// should be small enough to not cause objects to explode but large enough to satisfy constrains
			float baumgarteScalar = 0.1f;
			b = -(baumgarteScalar / PhysicsEngine::Instance()->GetDeltaTime())
				* distanceOffset;

			float xCoeff = -(reducedMass / dt) * stiffness;
			float vCoeff = -reducedMass * damping * constraintVel;
			float jn = xCoeff * distanceOffset + vCoeff * constraintVel;

			// Linear velocity impulse
			pnodeA->SetLinearVelocity(pnodeA->GetLinearVelocity() +
				abn * pnodeA->GetInverseMass() * jn);
			pnodeB->SetLinearVelocity(pnodeB->GetLinearVelocity() -
				abn * pnodeB->GetInverseMass() * jn);

			// Rotational velocity impulse
			pnodeA->SetAngularVelocity(pnodeA->GetAngularVelocity()
				+ pnodeA->GetInverseInertia() * Vector3::Cross(r1, abn * jn));
			pnodeB->SetAngularVelocity(pnodeB->GetAngularVelocity()
				- pnodeB->GetInverseInertia() * Vector3::Cross(r2, abn * jn));
		}

	}


	inline void SetStiffness(float s) { stiffness = s; }
	inline void SetDamping(float d) { damping = d; }
protected:
	PhysicsNode *pnodeA, *pnodeB;

	float   targetLength;

	Vector3 relPosA;
	Vector3 relPosB;

	// Stiffness and damping coefficients
	// Small values less than 1 result in soft bouncy springs
	// A value of 1.0 returns the spring to rest state in one simulation step.
	// Unstable for negative values and potentially unstable for values greater than 1
	// When n multiple springs are connected to the same particle coefficients should 
	// be limited to a maximum of 1 / (n + 1) according to https://www.gamedev.net/articles/programming/math-and-physics/towards-a-simpler-stiffer-and-more-stable-spring-r3227/
	float stiffness = 1.0f;
	float damping = 1.0f;

};