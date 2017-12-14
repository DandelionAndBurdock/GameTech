#include "Hazard.h"

#include <ncltech\HazardStates.h>

#include <ncltech\BehaviourTypes.h>
using namespace Steering;
Hazard::Hazard(std::string name) :
	AIObject(name)
{
	physicsNode = new PhysicsNode();

	fsm = new StateMachineManager<Hazard>(this);
	fsm->ChangeSuperState(Patrol::GetInstance());

}


Hazard::~Hazard()
{
	delete fsm;
}

void Hazard::SetPath(std::list<const GraphNode*> pathList) {
	path = std::vector<const GraphNode*>(std::begin(pathList), std::end(pathList));
	currentPathIndex = 0;
	physicsNode->SetPosition(path[0]->GetPos());
	std::vector<Vector3> pathPos;
	for (auto& node : path) {
		pathPos.push_back(node->GetPos());
	}
	physicsNode->AddSteeringBehaviour(FOLLOW_PATH);
	physicsNode->ChangeFollowWaypoints(pathPos);

}

void Hazard::Update(float dt) {
	fsm->Update(dt);
}

void Hazard::ReceiveMessage(const Messaging::Message& message) {
	fsm->ReceiveMessage(this, message);
}