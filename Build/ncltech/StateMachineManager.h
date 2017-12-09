#pragma once


#include "State.h"
#include "Messenger.h"

template <typename StateOwner>
class StateMachineManager {
public:
	StateMachineManager(StateOwner* owner);
	~StateMachineManager();

	void ChangeState(State<StateOwner>* state); 
	void Update(float dt);

	void ReceiveMessage(Messaging::Message msg);
protected:
	StateOwner*		   entity;		 // Entity running this FSM		
	State<StateOwner>* currentState; // Current state of the entity
};

