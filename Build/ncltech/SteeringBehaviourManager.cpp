#include "SteeringBehaviourManager.h"

#include "SteeringBehaviour.h"

#include "Pursuit.h"
#include "Seek.h"
#include "Wander.h"
#include "FollowPath.h"


#include "AIObject.h"

using namespace Steering;

SteeringBehaviourManager::SteeringBehaviourManager(PhysicsNode* owner)
{
	this->owner = owner;
}


SteeringBehaviourManager::~SteeringBehaviourManager()
{
	for (auto& behaviour : behaviours) {
		delete behaviour;
	}
}


void SteeringBehaviourManager::Update(float dt) {
	for (auto& behaviour : behaviours) {
		behaviour->Update(dt);
	}
}
Vector3 SteeringBehaviourManager::GetVelocity() {
	Vector3 velocity(0.0f);
	for (auto& behaviour : behaviours) {
		if (behaviour->IsActive())
			velocity += behaviour->GetVelocity();
	}
	return velocity;
}

void SteeringBehaviourManager::AddBehaviour(const BehaviourType b) {
	for (auto& behaviour : behaviours) {
		if (behaviour->GetType() == b) {
			return;
		}
	}

	switch (b) {
	case SEEK:
		behaviours.push_back(new Seek(owner));
		break;
	case WANDER:
		behaviours.push_back(new Wander(owner));
		break;
	case PURSUIT:
		behaviours.push_back(new Pursuit(owner));
		break;
	case FOLLOW_PATH:
		behaviours.push_back(new FollowPath(owner));
		break;
	default: 
		std::cout << "Warning: Unimplemented Behaviour" << std::endl;
	}

}


void SteeringBehaviourManager::SetSeekTarget(Vector3 target) {
	for (auto& behaviour : behaviours) {
		if (behaviour->GetType() == SEEK) {
			dynamic_cast<Seek*>(behaviour)->SetTarget(target);
		}
	}
}

void SteeringBehaviourManager::SetPursuitTarget(PhysicsNode* target) {
	for (auto& behaviour : behaviours) {
		if (behaviour->GetType() == PURSUIT) {
			dynamic_cast<Pursuit*>(behaviour)->SetTarget(target);
		}
	}
}

void SteeringBehaviourManager::SetFollowWaypoints(std::vector<Vector3>& waypoints) {
	for (auto& behaviour : behaviours) {
		if (behaviour->GetType() == FOLLOW_PATH) {
			dynamic_cast<FollowPath*>(behaviour)->SetWaypoints(waypoints);
		}
	}
}

void SteeringBehaviourManager::FollowPathOn() {
	for (auto& behaviour : behaviours) {
		if (behaviour->GetType() == FOLLOW_PATH) {
			behaviour->SetActive(true);
		}
	}
}

void SteeringBehaviourManager::FollowPathOff() {
	for (auto& behaviour : behaviours) {
		if (behaviour->GetType() == FOLLOW_PATH) {
			behaviour->SetActive(false);
		}
	}
}
void SteeringBehaviourManager::FollowPathLoop(bool isOn) {
	for (auto& behaviour : behaviours) {
		if (behaviour->GetType() == FOLLOW_PATH) {
			dynamic_cast<FollowPath*>(behaviour)->SetLooping(true);
		}
	}
}

void SteeringBehaviourManager::PursuitOn() {
	for (auto& behaviour : behaviours) {
		if (behaviour->GetType() == PURSUIT) {
			behaviour->SetActive(true);
		}
	}
}
void SteeringBehaviourManager::PursuitOff() {
	for (auto& behaviour : behaviours) {
		if (behaviour->GetType() == PURSUIT) {
			behaviour->SetActive(false);
		}
	}
}