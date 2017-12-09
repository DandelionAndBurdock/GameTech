#pragma once
#include <nclgl\Vector3.h>
#include "BehaviourTypes.h"

class GameObject;
class SteeringBehaviour
{
public:
	SteeringBehaviour(GameObject* entity);
	~SteeringBehaviour();

	virtual Vector3 GetVelocity() = 0;
	virtual void	Update(float dt) = 0;

	inline Steering::BehaviourType GetType() { return type; }

protected:
	Steering::BehaviourType type;

	GameObject* owner;
};

