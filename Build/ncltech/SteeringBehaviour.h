#pragma once
#include <nclgl\Vector3.h>
#include "BehaviourTypes.h"

class PhysicsNode;
class SteeringBehaviour
{
public:
	SteeringBehaviour(PhysicsNode* entity);
	~SteeringBehaviour();

	virtual Vector3 GetVelocity() = 0;
	virtual void	Update(float dt) = 0;

	inline Steering::BehaviourType GetType() { return type; }

	void SetActive(bool isOn) {	isActive = isOn; }
	bool IsActive() { return isActive; }
protected:
	Steering::BehaviourType type;
	bool isActive = true;

	PhysicsNode* owner;
};

