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


template <typename StateOwner>
StateMachineManager<StateOwner>::StateMachineManager(StateOwner* owner) :
	entity(owner), currentState(nullptr)
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
void StateMachineManager<StateOwner>::Update(float dt) {
	if (currentState) {
		currentState->Update(entity, dt);
	}
}

template <typename StateOwner>
void StateMachineManager<StateOwner>::ReceiveMessage(Messaging::Message msg) {
	if (currentState) {
		currentState->ReceiveMessage(msg);
	}
}


