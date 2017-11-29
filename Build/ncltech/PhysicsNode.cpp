#include "PhysicsNode.h"
#include "PhysicsEngine.h"


void PhysicsNode::IntegrateForVelocity(float dt)
{
	IntegrateLinearVelocity(dt);
	IntegrateAngularVelocity(dt);
}

void PhysicsNode::IntegrateLinearVelocity(float dt) {
	if (invMass <= 0.0f) {
		return;
	}

	// Update acceleration
	acceleration = Vector3(0.0f);
	acceleration += GetGravity();

	acceleration += force * invMass;

	// Update velocity
	lastLinVelocity = linVelocity;
	linVelocity += acceleration * dt;

	if (integrationType >= MIDPOINT) {
		CalculateRKLinCoefficients(dt);
	}


	if (damping) {
		linVelocity = linVelocity * PhysicsEngine::Instance()->GetDampingFactor();
	}

}

void PhysicsNode::IntegrateAngularVelocity(float dt) {
	if (invInertia.SumDiagonals() <= 0.0f) {
		return;
	}

	// Update acceleration
	lastAngAcceleration = angAcceleration;
	angAcceleration = Vector3(0.0f);
	angAcceleration += invInertia * torque;

	// Update velocity
	lastAngVelocity = angVelocity;
	angVelocity += angAcceleration * dt;

	// Apply damping
	if (damping) {
		angVelocity = angVelocity * PhysicsEngine::Instance()->GetDampingFactor();
	}

}

/* Between these two functions the physics engine will solve for velocity
   based on collisions/constraints etc. So we need to integrate velocity, solve 
   constraints, then use final velocity to update position. 
*/

void PhysicsNode::IntegrateForPosition(float dt)
{
	IntegrateLinearPosition(dt);
	IntegrateAngularPosition(dt);

	//Finally: Notify any listener's that this PhysicsNode has a new world transform.
	// - This is used by GameObject to set the worldTransform of any RenderNode's. 
	//   Please don't delete this!!!!!
	FireOnUpdateCallback();
}

void PhysicsNode::IntegrateLinearPosition(float dt) {

	lastPosition = position;
	switch (integrationType) {
	case SYMPLECTIC_EULER:
		position += linVelocity * dt;
		break;
	case EXPLICIT_EULER:
		position += lastLinVelocity * dt;
		break;
	case MIDPOINT:

	default:
		position += RKLinCoeffs[1];
	}
}

void PhysicsNode::IntegrateAngularPosition(float dt) {
	lastOrientation = orientation;
	switch (integrationType) {
	case SYMPLECTIC_EULER :
		orientation = orientation + Quaternion(angVelocity * dt * 0.5, 0.0f) * orientation;
		break;
	case EXPLICIT_EULER:
		orientation = orientation + Quaternion(lastAngVelocity * dt * 0.5, 0.0f) * orientation;
		break;
	}
	orientation.Normalise();
}

// Assumes no position dependence of the velocity
void PhysicsNode::CalculateRKLinCoefficients(float dt) {
	RKLinCoeffs[0] = lastLinVelocity * dt; // Explicit Euler
	RKLinCoeffs[1] = (lastLinVelocity + acceleration * dt / 2);
	//RKLinCoeffs[1] = (lastLinVelocity + acceleration * )
	//RKLinCoeffs[2] = (lastLinVelocity 
	
}

void PhysicsNode::CalculateRKAngCoefficients(float dt) {
	RKAngCoeffs[0] = lastLinVelocity * dt * dt; // Explicit Euler
	RKAngCoeffs[1] = (lastLinVelocity + acceleration * dt / 2) * dt;
}


Vector3 PhysicsNode::GetGravity() {
	// Direction
	Vector3 fieldDirection;
	if (gravAttractorPos) {
		fieldDirection = *gravAttractorPos - position;
		fieldDirection.Normalise();
	}
	else {
		fieldDirection = Vector3(0.0f, -1.0f, 0.0f);
	}


	// Strength
	return fieldDirection * PhysicsEngine::Instance()->GetGravity();

}

Vector3 PhysicsNode::PredictAcceleration(Vector3 pos) {
	Vector3 fieldDirection;
	if (gravAttractorPos) {
		fieldDirection = *gravAttractorPos - pos;
		fieldDirection.Normalise();
	}
	else {
		fieldDirection = Vector3(0.0f, -1.0f, 0.0f);
	}
	return fieldDirection * PhysicsEngine::Instance()->GetGravity();

}