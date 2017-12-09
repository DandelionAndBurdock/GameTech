#include "Server.h"

#include "NetworkConstants.h"
#include <nclgl\Vector3.h>


using namespace Packets;

Server::Server(int portNumber, int maxClients)
{
	isInitialised = serverNetwork.Initialize(portNumber, maxClients);
}


Server::~Server()
{
	serverNetwork.Release();
}


void Server::Update(float dt) {
	timeSinceBroadcast += dt;

	HandleIncomingTraffic(dt);
	if (timeSinceBroadcast >= UPDATE_TIMESTEP) {
		timeSinceBroadcast = 0.0f;
		BroadcastOutgoingTraffic();
	}

	rotation += 0.5f * 3.14f * dt;
}

void Server::HandleIncomingTraffic(float dt) {
	//Handle All Incoming Packets and Send any enqued packets
	serverNetwork.ServiceNetwork(dt, [&](const ENetEvent& evnt)
	{
		switch (evnt.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			printf("- New Client Connected\n");
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			ReceiveMessage(evnt);
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			printf("- Client %d has disconnected.\n", evnt.peer->incomingPeerID);
			break;
		}
	});
}

void Server::BroadcastOutgoingTraffic() {
	Vector3 pos = Vector3(
		cos(rotation) * 2.0f,
		1.5f,
		sin(rotation) * 2.0f);

	//Create the packet and broadcast it (unreliable transport) to all clients
	ENetPacket* position_update = enet_packet_create(&pos, sizeof(Vector3), 0);
	enet_host_broadcast(serverNetwork.m_pNetwork, 0, position_update);
}


void Server::ReceiveMessage(const ENetEvent& evnt) {
	PacketType* message = reinterpret_cast<PacketType*>(evnt.packet->data);
	switch (*message) {
	case TEST_PACKET:
		// Move pointer to the start of the data
		++message;
		HandleTestPacket(evnt.peer->incomingPeerID, message);
		break;
	}
	enet_packet_destroy(evnt.packet);
}

void Server::HandleTestPacket(int clientID, PacketType* message) {
	std::string* info = reinterpret_cast<std::string*>(message);
	std::cout << "Client " << clientID << " says " << *info;
}