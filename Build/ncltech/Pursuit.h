#pragma once

#include "SteeringBehaviour.h"
#include "GameObject.h"
class Seek;

class Pursuit : public SteeringBehaviour
{
public:
	Pursuit(PhysicsNode* entity);
	~Pursuit();

	Vector3 GetVelocity() override;
	void Update(float dt) override;

	inline void SetTarget(PhysicsNode* tgt) { target = tgt; }
protected:
	PhysicsNode* target = nullptr;
	Seek* seek = nullptr;;
};