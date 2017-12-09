#include "Seek.h"



Seek::Seek(AIObject* entity) :
	SteeringBehaviour(entity)
{
}


Seek::~Seek()
{
}

void Seek::Update(float dt) {
}

Vector3 Seek::GetVelocity() {
	Vector3 direction = Vector3(targetPos - owner->Physics()->GetPosition()).Normalise();
	// Hard code for now
	float maxSpeed = 2.0f;
	Vector3 desiredVelocity = direction * maxSpeed;

	return (desiredVelocity - owner->Physics()->GetLinearVelocity());
}
