#pragma once
#include <vector>

#include <nclgl\Vector3.h>

#include "BehaviourTypes.h"


class PhysicsNode;
class SteeringBehaviour;


class SteeringBehaviourManager
{
public:
	SteeringBehaviourManager(PhysicsNode* owner);
	~SteeringBehaviourManager();

	void Update(float dt);
	Vector3 GetVelocity();

	 void AddBehaviour(const Steering::BehaviourType b);

	 void SetSeekTarget(Vector3 target);
	 void SetPursuitTarget(PhysicsNode* target);

protected:

	std::vector<SteeringBehaviour*> behaviours;

	PhysicsNode* owner;

};

