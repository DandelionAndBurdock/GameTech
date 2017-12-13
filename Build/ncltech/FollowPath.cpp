#include "FollowPath.h"

#include "PhysicsNode.h"

#include "BehaviourTypes.h"
using namespace Steering;

FollowPath::FollowPath(PhysicsNode* entity) :
	SteeringBehaviour(entity), currentWaypoint(-1)
{
	type = FOLLOW_PATH;
}


FollowPath::~FollowPath()
{
	delete seek;
}

void FollowPath::Update(float dt) {
	if (currentWaypoint < 0) {
		return;
	}
	//TODO: Hardcode for now
	const float changeDistanceSq = 0.05f;
	if (Vector3::DistanceSq(waypoints[currentWaypoint], owner->GetPosition()) < changeDistanceSq) {
		if (currentWaypoint != waypoints.size() - 1) {
			++currentWaypoint;
			seek->SetTarget(waypoints[currentWaypoint]);
		}
	}
}

Vector3 FollowPath::GetVelocity() {
	if (seek) {
		return seek->GetVelocity();
	}
	else {
		return Vector3(0.0f);
	}
	
}

void FollowPath::SetWaypoints(std::vector<Vector3>& wayPoints) {
	waypoints = wayPoints;
	currentWaypoint = 0;
	seek = new Seek(owner);
}