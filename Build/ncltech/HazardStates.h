#pragma once

#include "State.h"
#include "AIObject.h"

class Pursue : public State<AIObject*>
{
public:
	Pursue();
	~Pursue();

	void Enter(GameObject* owner);
	void Update(GameObject* owner, float dt);
	void Exit(GameObject* owner);

	void ReceiveMessage(Messaging::Message msg);
};

