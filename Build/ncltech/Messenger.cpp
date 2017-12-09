#include "Messenger.h"

using namespace Messaging;

#include "AIObject.h"


Messenger::Messenger()
{
}


Messenger::~Messenger()
{
}


Messenger* Messenger::Instance() {
	static Messenger messenger;
	return &messenger;
}

void Messenger::AddMessageToQueue(const Messaging::Message& msg) {
	//Keep queue ordered at each insertion
	auto queuePos = std::upper_bound(messages.begin(), messages.end(), msg);
	//messages.insert(queuePos, msg);
}

void Messenger::Update(float dt) {
	// Update time stamps
	for (auto& msg : messages) {
		msg.Update(dt);
	}
	
	// Send any messages whose time stamps have expired
	for (auto& msg : messages) {
		if (msg.sendTimeDelay <= 0) {
			AIObject::GetObjectFromID(msg.receiever)->ReceiveMessage(msg);
		}
		else {
			break;
		}
	}
}