#pragma once

#include "Server.h"

class MazeGenerator;
class GraphNode;
class SearchAStar;

class MazeServer : public Server
{
public:
	MazeServer(int portNumber, int maxClients);
	~MazeServer();

protected:

	void ReceiveMessage(const ENetEvent& evnt) override;

	void HandleMazeParams(int clientID, Packets::PacketType* message);

	void RegenerateMaze(int clientID, Packets::PacketType* message);

	void BroadcastMazeStructure();

	void SendMazeRoute(int client);
	void HandleRouteRequest(int clientID, Packets::PacketType* message);

	MazeGenerator* maze = nullptr;

	SearchAStar* graphSearch = nullptr;
};


