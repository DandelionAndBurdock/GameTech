// Singleton class for sending messages between AI objects
#pragma once

#include <queue>

namespace Messaging{
	struct Message {
		int msg;				// Can be defined as an enum in context
		int receiever;			// Every game object has a unique id
		float sendTimeDelay;	//  How many seconds until message should be send
		void* extraInfo;		// Extra information e.g. a target position

		Message(int m, int r, int t = 0.0f, void* info = nullptr) :
			msg(m), receiever(r), sendTimeDelay(t), extraInfo(info)
		{}

		void Update(float dt) { sendTimeDelay -= dt; }

		friend bool operator<(const Message& lhs, const Message& rhs) {
			return lhs.sendTimeDelay < rhs.sendTimeDelay;
		}
	};
}


class Messenger
{
public:
	static Messenger* Instance();
	void AddMessageToQueue(const Messaging::Message& msg);

	void Update(float dt);

protected:
	// List of pending messages sorted by send time
	std::vector<Messaging::Message> messages;

	Messenger();
	~Messenger();
};

