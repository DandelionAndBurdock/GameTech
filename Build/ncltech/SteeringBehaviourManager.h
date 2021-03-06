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
	 void SetFollowWaypoints(std::vector<Vector3>& waypoints);

	 void FollowPathOn();
	 void FollowPathOff();
	 void FollowPathLoop(bool isOn);
	 void PursuitOn();
	 void PursuitOff();

protected:

	std::vector<SteeringBehaviour*> behaviours;

	PhysicsNode* owner;

};

