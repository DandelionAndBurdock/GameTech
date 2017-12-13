#include "Seek.h"

#include "BehaviourTypes.h"

#include "GameObject.h"

using namespace Steering;

Seek::Seek(PhysicsNode* entity) :
	SteeringBehaviour(entity)
{
	type = SEEK;
}


Seek::~Seek()
{
}

void Seek::Update(float dt) {
}

Vector3 Seek::GetVelocity() {
	Vector3 direction = Vector3(targetPos - owner->GetPosition()).Normalise();
	// Hard code for now
	float maxSpeed = 0.5f;
	Vector3 desiredVelocity = direction * maxSpeed;

	return (desiredVelocity - owner->GetLinearVelocity());
}
