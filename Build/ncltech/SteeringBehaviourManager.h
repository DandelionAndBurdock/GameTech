#pragma once
#include <vector>

#include <nclgl\Vector3.h>

class SteeringBehaviour;


class SteeringBehaviourManager
{
public:
	SteeringBehaviourManager();
	~SteeringBehaviourManager();

	void Update(float dt);
	Vector3 GetVelocity();

	inline void AddBehaviour(SteeringBehaviour* b) { behaviours.push_back(b); }

protected:

	std::vector<SteeringBehaviour*> behaviours;

};

