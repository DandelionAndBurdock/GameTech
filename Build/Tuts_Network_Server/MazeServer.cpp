#include "MazeServer.h"

#include <ncltech\Packets.h>
using namespace Packets;

#include <ncltech\MazeGenerator.h>
#include <ncltech\SearchAStar.h>

#include "Hazard.h"

#include "Avatar.h"

#include <sstream>

#include <map>

#include <ncltech\Messenger.h>

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
	BroadcastHazardPositions();
	BroadcastSecondaryAvatarPositions();
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
	case ADD_HAZARD_REQUEST:
		AddHazard();
		break;
	}

	Server::ReceiveMessage(evnt);
}

void MazeServer::HandleMazeParams(int clientID, Packets::PacketType* message) {
	std::cout << "Received maze parameters from client " << clientID << std::endl;

	if (!maze) {
		int* dim = reinterpret_cast<int*>(message);
		++message;

		float* density = reinterpret_cast<float*>(message);
		maze = new MazeGenerator();
		maze->Generate(*dim, *density);
		std::cout << "Generated maze" << std::endl;
	}
	BroadcastMazeStructure();
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

void MazeServer::SendNavMesh(int client, ENetPeer * peer) {
	//Get nav mesh
	std::vector<const GraphNode*> path = graphSearch->GetNavMesh();
	// Put into a string
	std::ostringstream ss;
	for (auto iter = path.begin(); iter != path.end(); ++iter) {
		ss << maze->GetIndexFromNode(*iter) << std::endl;
	}

	PacketCharArray mazeRoute(NAV_MESH);
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
		SendNavMesh(clientID, peer);
	}
}

void MazeServer::HandleAvatarRequest(int clientID, ENetPeer * peer, Packets::PacketType* message) {
	std::cout << "Received avatar creation request from client " << clientID << std::endl;

	auto avatarIter = std::find_if(avatars.begin(), avatars.end(), [peer](Avatar* a) { return (a->GetClient() == peer);});
	if (avatarIter == avatars.end()) {
		avatars.push_back(new Avatar(peer));
	}


	// Set avatar path
	char* indices = reinterpret_cast<char*>(message);
	std::string s = std::string(indices);
	std::istringstream ss(s);
	int startIdx, endIdx;
	ss >> startIdx >> endIdx;

	SetAvatarPath(clientID, peer, startIdx, endIdx);

	// Inform any pre-existing clients to add a secondary avatar
	for (auto& client : clients) {
		if (client != peer) {
			PacketInt avatar(ADD_SECONDARY_AVATAR, avatars.size()); // Need to make sure it is a free location
			ENetPacket* packet = enet_packet_create(&avatar, sizeof(avatar), 0);
			enet_peer_send(peer, 0, packet);
		}
	}
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
		SendNavMesh(clientID, peer);
		return true;
	}
}

void MazeServer::BroadcastAvatarPositions() {
	for (auto& avatar : avatars) {
		ENetPeer* sendClient = avatar->GetClient();
		PacketVec3 positionUpdate(AVATAR_POS, avatar->GetPosition());
		ENetPacket* position_update = enet_packet_create(&positionUpdate, sizeof(PacketVec3), 0);
		enet_peer_send(sendClient, 0, position_update);
	}
}


void MazeServer::BroadcastHazardPositions() {
	for (uint i = 0; i < hazards.size(); ++i) {
		PacketIntVec3 positionUpdate(HAZARD_POS, i, hazards[i]->Physics()->GetPosition());
		ENetPacket* position_update = enet_packet_create(&positionUpdate, sizeof(positionUpdate), 0);
		enet_host_broadcast(serverNetwork.m_pNetwork, 0, position_update);
	}
}

void MazeServer::AddHazard() {
	hazards.push_back(new Hazard("Hazard" + std::to_string(numHazards)));
	PhysicsEngine::Instance()->AddPhysicsObject(hazards[numHazards]->Physics());
	// Make Patrol Route
	int start = FindFreeNode();
	int end = FindFreeNode();
	graphSearch->FindBestPath(maze->GetNodeFromIndex(start), maze->GetNodeFromIndex(end));
	hazards[numHazards]->SetPath(graphSearch->GetFinalPath());
	++numHazards;
	// Broadcast location
	Packet hazard(ADD_HAZARD); // Need to make sure it is a free location
	ENetPacket* packet = enet_packet_create(&hazard, sizeof(hazard), 0);
	enet_host_broadcast(serverNetwork.m_pNetwork, 0, packet);
}


// Currently not working
int MazeServer::FindFreeNode() {
	return rand() % (maze->GetSize() * maze->GetSize());
}

void MazeServer::Update(float dt) {
	Avatar* avatar = nullptr;
	if (!avatars.empty()) {
		avatar = avatars[0];
	}

	for (auto& hazard : hazards) {
		hazard->Update(dt);
		if (avatar) {
			hazard->ReceiveMessage(Messaging::Message(0, -1, 0.0f, (void*)avatar));// Send hazard a message about avatar position(s)
		}
	}
	Server::Update(dt);
}


void MazeServer::OnClientConnection(ENetPeer* peer) {
	// Inform client about pre-exisiting avatars
	for (uint i = 0; i < avatars.size(); ++i) {
		if (avatars[i]->GetClient() != peer) {
			// Broadcast location
			PacketInt avatar(ADD_SECONDARY_AVATAR, i); // Need to make sure it is a free location
			ENetPacket* packet = enet_packet_create(&avatar, sizeof(avatar), 0);
			enet_peer_send(peer, 0, packet);
		}
	}

	clients.push_back(peer);
}


void MazeServer::BroadcastSecondaryAvatarPositions() {
	for (uint i = 0; i < avatars.size(); ++i) {
		for (auto& client : clients) {
			if (avatars[i]->GetClient() != client) {
				PacketIntVec3 avatarUpdate(SEC_AVATAR_UPDATE, i, avatars[i]->GetPosition());
				ENetPacket* packet = enet_packet_create(&avatarUpdate, sizeof(avatarUpdate), 0);
				enet_peer_send(client, 0, packet);
			}
		}
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