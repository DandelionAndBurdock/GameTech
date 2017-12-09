#include "Pursuit.h"

#include "Seek.h"
#include "BehaviourTypes.h"

using namespace Steering;

Pursuit::Pursuit(GameObject* entity) :
	SteeringBehaviour(entity),
	seek(new Seek(entity))
{
	type = PURSUIT;
}


Pursuit::~Pursuit()
{
	delete seek;
}

Vector3 Pursuit::GetVelocity() {
	// Crude pursuit for now -> Implement prediction of where the target should be 
	if (target) {
		return seek->GetVelocity();
	}
	else {
		return Vector3(0.0f);
	}

}

void Pursuit::Update(float dt) {
	if (target) {
		seek->SetTarget(target->Physics()->GetPosition());
	}

}