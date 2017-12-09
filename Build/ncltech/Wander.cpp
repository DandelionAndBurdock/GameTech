#include "Wander.h"



Wander::Wander(AIObject* entity) :
	SteeringBehaviour(entity),
	seek(new Seek(entity))
{
}


Wander::~Wander()
{
	delete seek;
}


void Wander::Update(float dt) {
	timeSinceTargetChange += dt;
	if (timeSinceTargetChange >= changeTargetTime) {
		timeSinceTargetChange -= changeTargetTime;
		targetPos = GetRandTargetPos();
		seek->SetTarget(targetPos);
	}

}
Vector3 Wander::GetVelocity() {
	return seek->GetVelocity();
}

// Hardcode for now
Vector3 Wander::GetRandTargetPos() {
	float x = (rand() % 10 - 10) / 10.0f;
	float z = (rand() % 10 - 10) / 10.0f;
	return Vector3(x, 2.0f, z);
}