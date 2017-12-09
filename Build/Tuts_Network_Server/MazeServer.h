#pragma once

#include "Server.h"

class MazeGenerator;

class MazeServer : public Server
{
public:
	MazeServer(int portNumber, int maxClients);
	~MazeServer();

protected:

	void ReceiveMessage(const ENetEvent& evnt) override;

	void HandleMazeParams(int clientID, Packets::PacketType* message);

	MazeGenerator* maze = nullptr;
};


