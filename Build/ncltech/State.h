// Abstract base class for states
#pragma once

#include "Messenger.h"

template <typename StateOwner>
class State {
public:
	virtual void Enter(StateOwner* owner) = 0;
	virtual void Update(StateOwner* owner, float dt) = 0;
	virtual void Exit(StateOwner* owner) = 0;

	virtual void ReceiveMessage(StateOwner* owner, Messaging::Message msg) = 0;
};