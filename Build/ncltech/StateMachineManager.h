#pragma once


#include "State.h"
#include "Messenger.h"

template <typename StateOwner>
class StateMachineManager {
public:
	StateMachineManager(StateOwner* owner);
	~StateMachineManager();

	void ChangeState(State<StateOwner>* state); 
	void ChangeSuperState(State<StateOwner>* state);
	void Update(float dt);

	void ReceiveMessage(StateOwner* owner, Messaging::Message msg);
protected:
	StateOwner*		   entity;		 // Entity running this FSM		
	State<StateOwner>* currentState; // Current state of the entity
	State<StateOwner>* superState;	 // My hacky solution to a hierachical state machine
};


template <typename StateOwner>
StateMachineManager<StateOwner>::StateMachineManager(StateOwner* owner) :
	entity(owner), currentState(nullptr), superState(nullptr)
{}

template <typename StateOwner>
StateMachineManager<StateOwner>::~StateMachineManager()
{}

template <typename StateOwner>
void StateMachineManager<StateOwner>::ChangeState(State<StateOwner>* newState) {
	if (currentState) {
		currentState->Exit(entity);
	}
	currentState = newState;

	newState->Enter(entity);
}

template <typename StateOwner>
void StateMachineManager<StateOwner>::ChangeSuperState(State<StateOwner>* newState) {
	if (superState) {
		superState->Exit(entity);
	}
	superState = newState;
	if (newState) {
		newState->Enter(entity);
	}
	
}


template <typename StateOwner>
void StateMachineManager<StateOwner>::Update(float dt) {
	if (currentState) {
		currentState->Update(entity, dt);
	}
	if (superState) {
		superState->Update(entity, dt);
	}
}

template <typename StateOwner>
void StateMachineManager<StateOwner>::ReceiveMessage(StateOwner* owner, Messaging::Message msg) {
	if (currentState) {
		currentState->ReceiveMessage(owner, msg);
	}
}



