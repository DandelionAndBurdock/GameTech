#include "AIObject.h"

#include "Messenger.h"
using namespace Messaging;

// Maps each object to its unique ID
std::map<int, AIObject*> AIObject::objectDirectory;

AIObject::AIObject(const std::string& name) :
	GameObject(name), fsm(this)
{
	static int ID = 0;
	uniqueID = ID;
	objectDirectory[ID++] = this;

	
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