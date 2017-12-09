#include "SteeringBehaviourManager.h"

#include "SteeringBehaviour.h"

SteeringBehaviourManager::SteeringBehaviourManager()
{
}


SteeringBehaviourManager::~SteeringBehaviourManager()
{
}


void SteeringBehaviourManager::Update(float dt) {
	for (auto& behaviour : behaviours) {
		behaviour->Update(dt);
	}
}
Vector3 SteeringBehaviourManager::GetVelocity() {
	Vector3 velocity(0.0f);
	for (auto& behaviour : behaviours) {
		velocity += behaviour->GetVelocity();
	}
	return velocity;
}