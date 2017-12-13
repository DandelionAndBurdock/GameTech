#include "MazeServer.h"

#include <ncltech\Packets.h>
using namespace Packets;

#include <ncltech\MazeGenerator.h>
#include <ncltech\SearchAStar.h>

#include "Avatar.h"

#include <sstream>

#include <map>

MazeServer::MazeServer(int portNumber, int maxClients) :
	Server(portNumber, maxClients),
	graphSearch(new SearchAStar())
{
}


MazeServer::~MazeServer()
{
	for (auto& avatar : avatars) {
		delete avatar;
	}
}


void MazeServer::BroadcastOutgoingTraffic() {
	BroadcastAvatarPositions();
	Server::BroadcastOutgoingTraffic();
}

void MazeServer::ReceiveMessage(const ENetEvent& evnt) {
	PacketType* message = reinterpret_cast<PacketType*>(evnt.packet->data);

	switch (*message) {
	case MAZE_PARAM:
		// Move pointer to the start of the data
		++message;
		HandleMazeParams(evnt.peer->incomingPeerID, message);
		break;
	case REGEN_MAZE:
		// Move pointer to the start of the data
		++message;
		RegenerateMaze(evnt.peer->incomingPeerID, message);
		break;
	case ROUTE_REQUEST:
		// Move pointer to the start of the data
		++message;
		HandleRouteRequest(evnt.peer->incomingPeerID, evnt.peer, message);
		break;
	case CREATE_AVATAR:
		// Move pointer to the start of the data
		++message;
		HandleAvatarRequest(evnt.peer->incomingPeerID, evnt.peer, message);
		break;
	}

	Server::ReceiveMessage(evnt);
}

void MazeServer::HandleMazeParams(int clientID, Packets::PacketType* message) {
	std::cout << "Received maze parameters from client " << clientID << std::endl;
	//TEMP
	//int* dim = reinterpret_cast<int*>(message);
	//++message;
	//
	//float* density = reinterpret_cast<float*>(message);
	//maze = new MazeGenerator();
	//
	//maze->Generate(10, *dim, *density);
	//PacketInt mazeParams(MAZE_STRUCTURE, 10);
	//ENetPacket* packet = enet_packet_create(&mazeParams, sizeof(mazeParams), 0);
	//enet_host_broadcast(serverNetwork.m_pNetwork, 0, packet);
	// End Temp


	if (!maze) {
		int* dim = reinterpret_cast<int*>(message);
		++message;

		float* density = reinterpret_cast<float*>(message);
		maze = new MazeGenerator();
		maze->Generate(*dim, *density);
		std::cout << "Generated maze" << std::endl;

	
		BroadcastMazeStructure();
	}
	
}


void MazeServer::RegenerateMaze(int clientID, Packets::PacketType* message) {
	std::cout << "Received regen maze request from client " << clientID << std::endl;
	SAFE_DELETE(maze);
	int* dim = reinterpret_cast<int*>(message);
	++message;

	float* density = reinterpret_cast<float*>(message);
	maze = new MazeGenerator();
	maze->Generate(*dim, *density);
	BroadcastMazeStructure();
	std::cout << "Generated maze" << std::endl;
}


void MazeServer::BroadcastMazeStructure() {
	std::cout << "Broadcasting Maze Structure" << std::endl;
	std::ostringstream ss;
	maze->Serialize(ss);

	PacketCharArray mazeParams(MAZE_STRUCTURE);
	memcpy(mazeParams.data, ss.str().c_str(), ss.str().size() * sizeof(char));
	ENetPacket* packet = enet_packet_create(&mazeParams, sizeof(mazeParams), 0);
	enet_host_broadcast(serverNetwork.m_pNetwork, 0, packet);
}

void MazeServer::SendMazeRoute(int client, ENetPeer * peer) {
	//Get route
	std::list<const GraphNode*> path = graphSearch->GetFinalPath();
	// Put into a string
	std::ostringstream ss;
	for (auto iter = path.begin(); iter != path.end(); ++iter) {
		ss << maze->GetIndexFromNode(*iter) << std::endl;
	}
	
	// Send
	//PacketString mazeRoute(MAZE_ROUTE, ss.str());
	
	PacketCharArray mazeRoute(MAZE_ROUTE);
	memcpy(mazeRoute.data, ss.str().c_str(), ss.str().size() * sizeof(char));

	ENetPacket* packet = enet_packet_create(&mazeRoute, sizeof(mazeRoute), 0);
	enet_peer_send(peer, 0, packet);
	std::cout << "Sent maze route to Client " << client << std::endl;
}

void  MazeServer::HandleRouteRequest(int clientID, ENetPeer * peer, Packets::PacketType* message) {
	std::cout << "Received route request from client " << clientID << std::endl;
	std::istringstream ss(*reinterpret_cast<std::string*>(message));
	int startIdx, endIdx;
	ss >> startIdx >> endIdx;
	
	graphSearch->FindBestPath(maze->GetNodeFromIndex(startIdx), maze->GetNodeFromIndex(endIdx));
	if (!SetAvatarPath(clientID, peer, startIdx, endIdx)) {
		SendMazeRoute(clientID, peer);
	}
}

void MazeServer::HandleAvatarRequest(int clientID, ENetPeer * peer, Packets::PacketType* message) {
	std::cout << "Received avatar creation request from client " << clientID << std::endl;
	avatars.push_back(new Avatar(peer));

	// Set avatar path
	char* indices = reinterpret_cast<char*>(message);
	std::string s = std::string(indices);
	std::istringstream ss(s);
	int startIdx, endIdx;
	ss >> startIdx >> endIdx;

	SetAvatarPath(clientID, peer, startIdx, endIdx);
}


bool MazeServer::SetAvatarPath(int clientID, ENetPeer * peer, int startIdx, int endIdx) {
	auto avatarIter = std::find_if(avatars.begin(), avatars.end(), [peer](Avatar* a) { return (a->GetClient() == peer);});
	if (avatarIter == avatars.end()) {
		return false;
	}
	else {
		graphSearch->FindBestPath(maze->GetNodeFromIndex(startIdx), maze->GetNodeFromIndex(endIdx));
		(*avatarIter)->SetPath(graphSearch->GetFinalPath());

		SendMazeRoute(clientID, peer);
		return true;
	}
}

void MazeServer::BroadcastAvatarPositions() {
	for (auto& avatar : avatars) {
		ENetPeer* sendClient = avatar->GetClient();
		PacketVec3 positionUpdate(AVATAR_POS, avatar->GetPosition());
		ENetPacket* position_update = enet_packet_create(&positionUpdate, sizeof(PacketVec3), 0);
		enet_peer_send(sendClient, 0, position_update);
		std::cout << "Sent avatar position update" << std::endl;
	}
}

//struct MyPacketNodeData;
//struct MyPacket
//{
//	int packet_type;
//	int numNodes;
//};
//MyPacket header;
//ENetPacket* packet = enet_packet_create(&header, sizeof(header) + sizeof(MyPacketNodeData) * 100, 0);

//MyPacket* header;
//header = reinterpret_cast<MyPacket*>(packetData); //memcpy this also otherwise when enet packet goers out of scope will delete mry|
//MyPacketNodeData* tmpArr = new MyPacketNodeData[header->numNodes];
//memcpy(tmpArr, reinterpret_cast<char*>(packetData) + sizeof(MyPacket), sizeof(MyPacketNodeData) * header->numNodes);