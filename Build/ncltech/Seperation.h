#pragma once

#include "SteeringBehaviour.h"
class Seperation : public SteeringBehaviour
{
public:
	Seperation(PhysicsNode* entity);
	~Seperation();

	Vector3 GetVelocity() override;
	void	Update(float dt) override;
};

