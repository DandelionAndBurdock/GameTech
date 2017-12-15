#include "Avatar.h"
#include <ncltech\PhysicsEngine.h>

#include <ncltech\PhysicsNode.h>

#include <ncltech\BehaviourTypes.h>
using namespace Steering;

Avatar::Avatar(ENetPeer* client) :
	client(client)
{
	physics = new PhysicsNode();
	PhysicsEngine::Instance()->AddPhysicsObject(physics);
}


Avatar::~Avatar()
{
	SAFE_DELETE(physics);
}


void Avatar::SetPath(std::list<const GraphNode*> pathList) {
	path = std::vector<const GraphNode*>(std::begin(pathList), std::end(pathList));
	currentPathIndex = 0;
	physics->SetPosition(path[0]->GetPos());
	std::cout << path[0]->GetPos() << std::endl;
	physics->AddSteeringBehaviour(FOLLOW_PATH);
	physics->FollowPathOn();
	std::vector<Vector3> pathPos;
	for (auto& node : path) {
		pathPos.push_back(node->GetPos());
	}
	physics->ChangeFollowWaypoints(pathPos);
	physics->SetLinearVelocity(Vector3(0.0f));

}


Vector3 Avatar::GetPosition() {
	return physics->GetPosition();
}

Vector3 Avatar::GetVelocity() {
	return physics->GetLinearVelocity();
}