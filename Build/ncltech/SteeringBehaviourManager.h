#pragma once
#include <vector>

#include <nclgl\Vector3.h>

#include "BehaviourTypes.h"


class GameObject;
class SteeringBehaviour;


class SteeringBehaviourManager
{
public:
	SteeringBehaviourManager(GameObject* owner);
	~SteeringBehaviourManager();

	void Update(float dt);
	Vector3 GetVelocity();

	 void AddBehaviour(const Steering::BehaviourType b);

	 void SetSeekTarget(Vector3 target);
	 void SetPursuitTarget(GameObject* target);

protected:

	std::vector<SteeringBehaviour*> behaviours;

	GameObject* owner;

};

