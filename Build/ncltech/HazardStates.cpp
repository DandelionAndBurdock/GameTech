#include "HazardStates.h"

#include "BehaviourTypes.h"
using namespace Steering;

Pursue* Pursue::GetInstance() {
	static Pursue instance;
	return &instance;
}

Pursue::Pursue()
{
}


Pursue::~Pursue()
{
}


void Pursue::Enter(Hazard* owner) {
	return;
}

void Pursue::Update(Hazard* owner, float dt) {
	return;
}

void Pursue::Exit(Hazard* owner) {
	return;
}

void Pursue::ReceiveMessage(Messaging::Message msg) {
	return;
}





FollowHazardPath* FollowHazardPath::GetInstance() {
	static FollowHazardPath instance;
	return &instance;
}

FollowHazardPath::FollowHazardPath() {

}

FollowHazardPath::~FollowHazardPath() {

}

void FollowHazardPath::Enter(Hazard* owner) {
	owner->Physics()->AddSteeringBehaviour(FOLLOW_PATH);
	owner->Physics()->FollowPathOn();
	owner->Physics()->FollowPathLoop(true);
}

void FollowHazardPath::Update(Hazard* owner, float dt) {
}

void FollowHazardPath::Exit(Hazard* owner) {
	owner->Physics()->FollowPathOff();
}

void FollowHazardPath::ReceiveMessage(Messaging::Message msg) {

}



Idle* Idle::GetInstance() {
	static Idle instance;
	return &instance;
}

Idle::Idle() {

}

Idle::~Idle() {

}

void Idle::Enter(Hazard* owner) {
	owner->Physics()->SetLinearVelocity(0.0f);
}

void Idle::Update(Hazard* owner, float dt) {
}

void Idle::Exit(Hazard* owner) {
}

void Idle::ReceiveMessage(Messaging::Message msg) {
}


Patrol* Patrol::GetInstance() {
	static Patrol instance;
	return &instance;
}

Patrol::Patrol() {

}

Patrol::~Patrol() {

}

void Patrol::Enter(Hazard* owner) {
	owner->GetFSM()->ChangeState(FollowHazardPath::GetInstance());
	isIdle = false;
	isFollowingPath = true;
}

void Patrol::Update(Hazard* owner, float dt) {
	timeSinceTransition += dt;

	if (isFollowingPath && timeSinceTransition >= pathTime) {
		timeSinceTransition -= pathTime;
		owner->GetFSM()->ChangeState(Idle::GetInstance());
		isIdle = true;
		isFollowingPath = false;
	 }

	if (isIdle && timeSinceTransition >= idleTime) {
		timeSinceTransition -= idleTime;
		owner->GetFSM()->ChangeState(FollowHazardPath::GetInstance());
		isIdle = false;
		isFollowingPath = true;
	}
}

void Patrol::Exit(Hazard* owner) {
}

void Patrol::ReceiveMessage(Messaging::Message msg) {
}