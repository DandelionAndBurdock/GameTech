#include "AIObject.h"

#include <nclgl\Vector3.h>
#include "Messenger.h"
using namespace Messaging;

// Maps each object to its unique ID
std::map<int, AIObject*> AIObject::objectDirectory;
// List of objects in scene
std::vector<AIObject*> AIObject::sceneObjects;

AIObject::AIObject(const std::string& name) :
	GameObject(name), fsm(this)
{
	static int ID = 0;
	uniqueID = ID;
	objectDirectory[ID++] = this;

	sceneObjects.push_back(this);
}


AIObject::~AIObject()
{
}

void AIObject::ReceiveMessage(const Messaging::Message& message) {
	fsm.ReceiveMessage(message);
}


AIObject* AIObject::GetObjectFromID(int ID) {
	if (objectDirectory.find(ID) != objectDirectory.end()) {
		return objectDirectory[ID];
	}
	else {
		return nullptr;
	}
}

// Hardcode a brute force approach for now -> better to use something like an octree
void AIObject::FindNeighbours(float radius, std::vector<AIObject*> excluding) {
	Vector3 pos = this->Physics()->GetPosition();

	for (const auto& object : sceneObjects) {
		if (object == this) {
			continue;
		}
		if (std::find(excluding.cbegin(), excluding.cend(), object) == excluding.cend()) {
			if (Vector3::DistanceSq(pos, object->Physics()->GetPosition()) < radius * radius) {
				neighbours.push_back(object);
			}
			
		}
	}
}

//TODO: Move steering manager from physics node to here so only updating neighbours if group behaviour is on
//TODO: Not necessary to update neighbours every frame
void AIObject::Update(float dt) {
	//GetNeighbours(2.0f);
}