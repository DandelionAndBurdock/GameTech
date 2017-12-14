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

	virtual void HandleIncomingTraffic(float dt);
	virtual void BroadcastOutgoingTraffic();

	virtual void OnClientConnection(ENetPeer* peer) {}
	virtual void ReceiveMessage(const ENetEvent& evnt);

	virtual void HandleTestPacket(int clientID, Packets::PacketType* message);

	// Temporary: Useful to check server is still working in early development
	float rotation = 0.0f;
	// Time since last broadcast
	float timeSinceBroadcast = 0.0f;
};

