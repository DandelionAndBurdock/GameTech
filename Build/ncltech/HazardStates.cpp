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
