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
	acceleration += PhysicsEngine::Instance()->GetGravity();

	acceleration += force * invMass;

	// Update velocity
	lastLinVelocity = linVelocity;
	linVelocity += acceleration * dt;

	if (midpointMethod) {
		CalculateRKLinCoefficients(dt);
	}
	

	// Apply damping
	linVelocity = linVelocity * PhysicsEngine::Instance()->GetDampingFactor();
}

void PhysicsNode::IntegrateAngularVelocity(float dt) {
	if (invInertia.SumDiagonals() <= 0.0f) {
		return;
	}

	// Update acceleration
	lastAngAcceleration = angAcceleration;
	angAcceleration = Vector3(0.0f);
	if (implicitEuler || symplecticEuler) {
		angAcceleration += invInertia * torque;
	}

	// Update velocity
	lastAngVelocity = angVelocity;
	angVelocity += angAcceleration * dt;

	// Apply damping
	angVelocity = angVelocity * PhysicsEngine::Instance()->GetDampingFactor();
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
	if (symplecticEuler) {
		position += linVelocity * dt;
	}
	else if (implicitEuler) {
		position += RKLinCoeffs[0];
	}
	else if (midpointMethod) {
		position += RKLinCoeffs[1];
	}
}

void PhysicsNode::IntegrateAngularPosition(float dt) {
	lastOrientation = orientation;
	if (symplecticEuler) {
		orientation = orientation + Quaternion(angVelocity * dt * 0.5, 0.0f) * orientation;
	}
	else if (implicitEuler) {
		orientation = orientation + Quaternion(lastAngVelocity * dt * 0.5, 0.0f) * orientation;
	}
	orientation.Normalise();
}

// Assumes no position dependence of the velocity
void PhysicsNode::CalculateRKLinCoefficients(float dt) {
	RKLinCoeffs[0] = lastLinVelocity * dt * dt; // Explicit Euler
	RKLinCoeffs[1] = (lastLinVelocity + acceleration * dt / 2) * dt;
	//RKLinCoeffs[2] = (lastLinVelocity 
	
}

void PhysicsNode::CalculateRKAngCoefficients(float dt) {
	RKAngCoeffs[0] = lastLinVelocity * dt * dt; // Explicit Euler
	RKAngCoeffs[1] = (lastLinVelocity + acceleration * dt / 2) * dt;
}