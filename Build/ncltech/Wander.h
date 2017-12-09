#pragma once
// Place a random target point on the world and seek towards it
// By changing the target point with time will cause the object to
// wander

#include "SteeringBehaviour.h"
#include "Seek.h"

class Wander : public SteeringBehaviour
{
public:
	Wander(AIObject* entity);
	~Wander();

	void Update(float dt) override;
	Vector3 GetVelocity() override;

protected:
	float changeTargetTime = 1.0f;
	float timeSinceTargetChange = 0.0f;

	Vector3 targetPos = Vector3(0.0f);

	Vector3 GetRandTargetPos();

	Seek* seek;
};