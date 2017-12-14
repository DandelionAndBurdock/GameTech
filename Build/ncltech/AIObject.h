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


	virtual void ReceiveMessage(const Messaging::Message& message);

	static AIObject* GetObjectFromID(int ID);

	virtual void Update(float dt);
	
	inline std::vector<AIObject*>& GetNeighbours() { return neighbours; }
protected:
	int uniqueID;

	//StateMachineManager<AIObject> fsm;

	void FindNeighbours(float radius, std::vector<AIObject*> excluding = std::vector<AIObject*>());

	//TODO: Make an entity manager class ***********
	// Maps each object to its unique ID
	static std::map<int, AIObject*> objectDirectory;
	// List of objects in scene
	static std::vector<AIObject*> sceneObjects;
	// List of neighbours within a certain radius
	std::vector<AIObject*> neighbours;
	//***********
};

