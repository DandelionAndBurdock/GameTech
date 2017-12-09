#pragma once

#include "GameObject.h"
#include "StateMachineManager.h"
#include "Messenger.h"
#include <string>
#include <map>

class Message;


class AIObject : public GameObject
{
public:
	AIObject(const std::string& name);
	~AIObject();


	void ReceiveMessage(const Messaging::Message& message);

	static AIObject* GetObjectFromID(int ID);
protected:
	int uniqueID;

	StateMachineManager<AIObject> fsm;

	// Maps each object to its unique ID
	static std::map<int, AIObject*> objectDirectory;
};

