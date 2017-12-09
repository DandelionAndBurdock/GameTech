#include "MazeServer.h"

#include <ncltech\Packets.h>
using namespace Packets;

#include <ncltech\MazeGenerator.h>

#include <sstream>

MazeServer::MazeServer(int portNumber, int maxClients) :
	Server(portNumber, maxClients)
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

		std::stringstream ss(std::stringstream::out);
		maze->Serialize(ss);

		//TODO: Put in own function rename packet check position broadcast
		PacketString mazeParams(MAZE_STRUCTURE, ss.str());
		ENetPacket* packet = enet_packet_create(&mazeParams, sizeof(mazeParams), 0);
		enet_host_broadcast(serverNetwork.m_pNetwork, 0, packet);
	}
	
}