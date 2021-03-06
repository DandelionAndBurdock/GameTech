#include "PhysicsNode.h"
#include "PhysicsEngine.h"

// Objects with a velocity less than the rest velocity are considered to be at rest
#define REST_VELOCITY_SQUARE 0.1F

#include "SteeringBehaviourManager.h"

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
		break;
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
	return fieldDirection * PhysicsEngine::Instance()->GetGravity() * gravityBoost;

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

void PhysicsNode::IntegratePreSolver(float dt) {
	if (steering) {
		linVelocity += steering->GetVelocity();
	}
	if (integrationType == NONE) {
		return;
	}
	if (integrationType == MIDPOINT || integrationType == RK4) { // Will not work for collisions in this framework
// If had a velocity or explicitly time dependent acceleration would also factor this in here

		// Start of the interval
		Vector3 dx1 = linVelocity * dt;
		Vector3 dv1 = GetGravity() * dt;

		// Midpoint -> Looks wrong??
		Vector3 dx2 = (linVelocity + dv1 / 2) * dt;
		Vector3 dv2 = PredictAcceleration(position + dx1 / 2) * dt;

		position += dx2;
		linVelocity += dv2;

		// DAMPING
	}
	else {
		IntegrateForVelocity(dt);
	}

	//Finally: Notify any listener's that this PhysicsNode has a new world transform.
	// - This is used by GameObject to set the worldTransform of any RenderNode's. 
	//   Please don't delete this!!!!!
	FireOnUpdateCallback();
}
void PhysicsNode::IntegratePostSolver(float dt) {
	if (integrationType == NONE) {
		return;
	}
	if (integrationType < MIDPOINT) { // Will not work for collisions in this framework
		IntegrateForPosition(dt);
	}
}


void PhysicsNode::AddSteeringBehaviour(Steering::BehaviourType b) {
	if (!steering) {
		steering = new SteeringBehaviourManager(this);
	}
	steering->AddBehaviour(b);
}

void PhysicsNode::ChangeSeekTarget(Vector3 target) {
	steering->SetSeekTarget(target);
}

void  PhysicsNode::ChangePursuitTarget(PhysicsNode* target) {
	steering->SetPursuitTarget(target);
}


void PhysicsNode::ChangeSteeringSpeed(float vel) {
	//steering->SetVelocity(vel);
}

void PhysicsNode::ChangeFollowWaypoints(std::vector<Vector3>& waypoints) {
	steering->SetFollowWaypoints(waypoints);
}

void PhysicsNode::FollowPathOn() {
	steering->FollowPathOn();
}
void PhysicsNode::FollowPathOff() {
	steering->FollowPathOff();
}
void PhysicsNode::FollowPathLoop(bool isOn) {
	steering->FollowPathLoop(isOn);
}

void PhysicsNode::PursuitOn() {
	steering->PursuitOn();
}
void PhysicsNode::PursuitOff() {
	steering->PursuitOn();
}


bool PhysicsNode::IsStationary() const {
	return Vector3::Dot(linVelocity, linVelocity) < REST_VELOCITY_SQUARE;
}