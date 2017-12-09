#include "StateMachineManager.h"

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