#pragma once

#include <ncltech\NetworkBase.h>
#include <ncltech\Packets.h>
class Server
{
public:
	Server(int portNumber, int maxClients);
	~Server();

	void Update(float dt);

	inline bool IsInitalised() { return isInitialised; }

protected:
	NetworkBase serverNetwork;

	bool isInitialised = false;

	void HandleIncomingTraffic(float dt);
	void BroadcastOutgoingTraffic();

	void ReceiveMessage(const ENetEvent& evnt);

	void HandleTestPacket(int clientID, Packets::PacketType* message);

	// Temporary: Useful to check server is still working in early development
	float rotation = 0.0f;
	// Time since last broadcast
	float timeSinceBroadcast = 0.0f;
};

