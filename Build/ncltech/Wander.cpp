#include "Wander.h"

using namespace Steering;

#include "GameObject.h"

Wander::Wander(GameObject* entity) :
	SteeringBehaviour(entity),
	seek(new Seek(entity))
{
	type = WANDER;
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
		seek->SetTarget(owner->Physics()->GetPosition() + targetPos);
	}

}
Vector3 Wander::GetVelocity() {
	return seek->GetVelocity();
}

// Hardcode for now -> Should change to circle method later
Vector3 Wander::GetRandTargetPos() {

	float x = (rand() % 100 - 50) / 10.0f;
	float z = (rand() % 100 - 50) / 10.0f;
	if (x < 0.0f) {
		x -= 1.0f;
	}
	else {
		x += 1.0f;
	}
	std::cout << x << " " << z << std::endl;
	return Vector3(x, 0.0f, z);
}