#include "MazeServer.h"

#include <ncltech\Packets.h>
using namespace Packets;

#include <ncltech\MazeGenerator.h>
#include <ncltech\SearchAStar.h>

#include <sstream>

MazeServer::MazeServer(int portNumber, int maxClients) :
	Server(portNumber, maxClients),
	graphSearch(new SearchAStar())
{
}


MazeServer::~MazeServer()
{
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
		//GraphNode* start = reinterpret_cast<GraphNode*>(message);
		//++message;
		//GraphNode* end = reinterpret_cast<GraphNode*>(message);
		//ss.read();
		HandleRouteRequest(evnt.peer->incomingPeerID, message);
		break;
	}

	Server::ReceiveMessage(evnt);
}

void MazeServer::HandleMazeParams(int clientID, Packets::PacketType* message) {
	std::cout << "Received maze parameters from client " << clientID << std::endl;
	//TEMP
	int* dim = reinterpret_cast<int*>(message);
	++message;

	float* density = reinterpret_cast<float*>(message);
	maze = new MazeGenerator();
	maze->Generate(10, *dim, *density);
	PacketInt mazeParams(MAZE_STRUCTURE, 10);
	ENetPacket* packet = enet_packet_create(&mazeParams, sizeof(mazeParams), 0);
	enet_host_broadcast(serverNetwork.m_pNetwork, 0, packet);
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
	if (maze) {
		delete maze;
	}
	int* dim = reinterpret_cast<int*>(message);
	++message;

	float* density = reinterpret_cast<float*>(message);
	maze = new MazeGenerator();
	maze->Generate(*dim, *density);
	std::cout << "Generated maze" << std::endl;
}


void MazeServer::BroadcastMazeStructure() {
	std::cout << "Broadcasting Maze Structure" << std::endl;
	std::stringstream ss(std::stringstream::out);
	maze->Serialize(ss);


	PacketString mazeParams(MAZE_STRUCTURE, ss.str());
	ENetPacket* packet = enet_packet_create(&mazeParams, sizeof(mazeParams), 0);
	enet_host_broadcast(serverNetwork.m_pNetwork, 0, packet);
}


void MazeServer::SendMazeRoute(int client) {
	std::list<const GraphNode*> path = graphSearch->GetFinalPath();
	// Put into a string
	
	// Send

}

void  MazeServer::HandleRouteRequest(int clientID, Packets::PacketType* message) {
	std::cout << "Received route request from client " << clientID << std::endl;
	std::istringstream ss(*reinterpret_cast<std::string*>(message));
	int startIdx, endIdx;
	ss >> startIdx >> endIdx;
	
	graphSearch->FindBestPath(maze->GetNodeFromIndex(startIdx), maze->GetNodeFromIndex(endIdx));
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