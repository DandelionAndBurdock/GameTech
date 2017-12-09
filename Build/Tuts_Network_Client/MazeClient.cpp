#include "MazeClient.h"

#include <ncltech\Packets.h>
using namespace Packets;

#include <ncltech\MazeGenerator.h>
#include <ncltech\MazeRenderer.h>

MazeClient::MazeClient(std::string name) :
	Net1_Client(name)
{
}


MazeClient::~MazeClient()
{
}

void MazeClient::OnConnection() {
	Net1_Client::OnConnection();

	// Send server maze parameters
	const int mazeDim = 6;
	float mazeDensity = 0.5f;

	PacketIntFloat mazeParams(MAZE_PARAM, mazeDim, mazeDensity);
	ENetPacket* packet = enet_packet_create(&mazeParams, sizeof(mazeParams), 0);
	enet_peer_send(serverConnection, 0, packet);
}


void MazeClient::ReceiveMessage(const ENetEvent& evnt) {
	PacketType* message = reinterpret_cast<PacketType*>(evnt.packet->data);
	switch (*message) {
	case POS_DATA:
		// Move pointer to the start of the data
		++message;
		HandlePosData(message);
		break;
	case MAZE_STRUCTURE:
		++message;
		HandleMazeStructure(message);
		break;
	default:
		std::cout << "Recieved Uncategorised Network Packet!" << std::endl;
	}
}

void MazeClient::HandlePosData(PacketType* message) {
	Vector3* pos = reinterpret_cast<Vector3*>(message);
	box->Physics()->SetPosition(*pos);
}

void MazeClient::HandleMazeStructure(Packets::PacketType* message) {
	if (!mazeGenerator) {
		std::string* mazeData = reinterpret_cast<std::string*>(message);
		std::stringstream ss(*mazeData, std::stringstream::in);
		mazeGenerator = new MazeGenerator();
		mazeGenerator->Deserialize(ss);
		std::cout << "Client has received maze parameters" << std::endl;
	}
}