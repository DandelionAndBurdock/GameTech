#pragma once
// Directs entity towards a target position
#include "SteeringBehaviour.h"

class Seek : public SteeringBehaviour
{
public:
	Seek(AIObject* entity);
	~Seek();

	void Update(float dt) override;
	Vector3 GetVelocity() override;

	inline void SetTarget(Vector3 target) { targetPos = target; }

protected:
	Vector3 targetPos = Vector3(0.0f);
};


