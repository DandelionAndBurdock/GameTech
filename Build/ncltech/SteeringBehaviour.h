#pragma once
#include <nclgl\Vector3.h>
#include "BehaviourTypes.h"

#include "AIObject.h"
class SteeringBehaviour
{
public:
	SteeringBehaviour(AIObject* entity);
	~SteeringBehaviour();

	virtual Vector3 GetVelocity() = 0;
	virtual void	Update(float dt) = 0;

protected:
	Steering::BehaviourType type;

	AIObject* owner;
};

