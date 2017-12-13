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
	physics->AddSteeringBehaviour(FOLLOW_PATH);
	std::vector<Vector3> pathPos;
	for (auto& node : path) {
		pathPos.push_back(node->GetPos());
	}
	physics->ChangeFollowWaypoints(pathPos);

}


Vector3 Avatar::GetPosition() {
	return physics->GetPosition();
}
