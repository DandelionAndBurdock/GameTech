#pragma once

#include "SteeringBehaviour.h"
#include "Seek.h"

#include <vector>
#include <nclgl\Vector3.h>

class FollowPath : public SteeringBehaviour
{
public:
	FollowPath(PhysicsNode* entity);
	~FollowPath();

	void Update(float dt) override;
	Vector3 GetVelocity() override;

	void SetWaypoints(std::vector<Vector3>& wayPoints);
	
protected:
	std::vector<Vector3> waypoints;
	int currentWaypoint;
	Seek* seek;
	
	// Goes backwards and forwards along the path
	bool looping = false;
	bool increasing = true;
};

