#pragma once

#include "Server.h"

class MazeGenerator;
class GraphNode;
class SearchAStar;
class Avatar;
class Hazard;

class MazeServer : public Server
{
public:
	MazeServer(int portNumber, int maxClients);
	~MazeServer();

	void Update(float dt);

	void BroadcastOutgoingTraffic() override;
protected:

	void ReceiveMessage(const ENetEvent& evnt) override;

	void HandleMazeParams(int clientID, Packets::PacketType* message);

	void RegenerateMaze(int clientID, Packets::PacketType* message);

	void BroadcastMazeStructure();
	void BroadcastAvatarPositions();
	void BroadcastHazardPositions();


	void SendMazeRoute(int client, ENetPeer * peer);
	void HandleRouteRequest(int clientID, ENetPeer * peer, Packets::PacketType* message);
	void HandleAvatarRequest(int clientID, ENetPeer * peer, Packets::PacketType* message);

	bool SetAvatarPath(int clientID, ENetPeer * peer, int startIdx, int endIdx);

	// Returns the index of a random node which is unoccupied
	int FindFreeNode();

	MazeGenerator* maze = nullptr;

	SearchAStar* graphSearch = nullptr;

	std::vector<Avatar*> avatars;

	// Hazards
	std::vector<Hazard*> hazards;
	int numHazards = 0; 
	void AddHazard();
};


