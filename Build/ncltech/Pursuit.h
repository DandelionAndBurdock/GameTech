#pragma once

#include "SteeringBehaviour.h"
#include "GameObject.h"
class Seek;

class Pursuit : public SteeringBehaviour
{
public:
	Pursuit(GameObject* entity);
	~Pursuit();

	Vector3 GetVelocity() override;
	void Update(float dt) override;

	inline void SetTarget(GameObject* tgt) { target = tgt; }
protected:
	GameObject* target = nullptr;
	Seek* seek = nullptr;;
};