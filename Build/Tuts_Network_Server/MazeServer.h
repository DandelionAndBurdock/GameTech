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
	void BroadcastSecondaryAvatarPositions();

	void SendMazeRoute(int client, ENetPeer * peer);
	void SendNavMesh(int client, ENetPeer * peer);
	void HandleRouteRequest(int clientID, ENetPeer * peer, Packets::PacketType* message);
	void HandleAvatarRequest(int clientID, ENetPeer * peer, Packets::PacketType* message);



	void OnClientConnection(ENetPeer* peer) override;

	bool SetAvatarPath(int clientID, ENetPeer * peer, int startIdx, int endIdx);

	// Returns the index of a random node which is unoccupied
	int FindFreeNode();

	MazeGenerator* maze = nullptr;

	SearchAStar* graphSearch = nullptr;

	std::vector<Avatar*> avatars;

	// List of connected clients
	std::vector<ENetPeer*> clients;
	// Hazards
	std::vector<Hazard*> hazards;
	int numHazards = 0; 
	void AddHazard();
};


