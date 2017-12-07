#pragma once

#include <ncltech\NetworkBase.h>


class Client
{
public:
	Client();
	~Client();

	void Update(float dt);
protected:
	void ProcessNetworkEvent(const ENetEvent& evnt);

	NetworkBase network;
	ENetPeer*	serverConnection;
};

