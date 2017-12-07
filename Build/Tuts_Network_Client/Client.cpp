#include "Client.h"

#include <nclgl\NCLDebug.h>
#include <ncltech\Packets.h>
using namespace Packet;

const Vector3 status_color3 = Vector3(1.0f, 0.6f, 0.6f);
const Vector4 status_color = Vector4(status_color3.x, status_color3.y, status_color3.z, 1.0f);

Client::Client() :
	serverConnection(nullptr)
{
	//Initialize Client Network
	if (network.Initialize(0))
	{
		NCLDebug::Log("Network: Initialized!");

		//Attempt to connect to the server on localhost:1234
		serverConnection = network.ConnectPeer(127, 0, 0, 1, 1234);
		NCLDebug::Log("Network: Attempting to connect to server.");
	}
}

void Client::Update(float dt) {
	//Update Network
	auto callback = std::bind(
		&Client::ProcessNetworkEvent,	// Function to call
		this,								// Associated class instance
		std::placeholders::_1);				// Where to place the first parameter
	network.ServiceNetwork(dt, callback);
}


Client::~Client()
{
	//Send one final packet telling the server we are disconnecting
	// - We are not waiting to resend this, so if it fails to arrive
	//   the server will have to wait until we time out naturally
	enet_peer_disconnect_now(serverConnection, 0);

	//Release network and all associated data/peer connections
	network.Release();
	serverConnection = NULL;
}


void Client::ProcessNetworkEvent(const ENetEvent& evnt) {
	switch (evnt.type)
	{
		//New connection request or an existing peer accepted our connection request
	case ENET_EVENT_TYPE_CONNECT:
	{
		if (evnt.peer == serverConnection)
		{
			NCLDebug::Log(status_color3, "Network: Successfully connected to server!");

			//Send a 'hello' packet
			PacketString testPacket;
			testPacket.message = TEST_PACKET;
			testPacket.str = "Hellooooo!";
			ENetPacket* packet = enet_packet_create(&testPacket, sizeof(PacketString), 0);
			enet_peer_send(serverConnection, 0, packet);
		}
	}
	break;


	//Server has sent us a new packet
	case ENET_EVENT_TYPE_RECEIVE:
	{
		PacketType* message = reinterpret_cast<PacketType*>(evnt.packet->data);

		switch (*message) {
		case POS_DATA:
			// Advance pointer to the start of the message data
			++message;
			Vector3* pos = reinterpret_cast<Vector3*>(message);
			//box->Physics()->SetPosition(*pos);
			enet_packet_destroy(evnt.packet);
			break;
		}
	}
	break;


	//Server has disconnected
	case ENET_EVENT_TYPE_DISCONNECT:
	{
		NCLDebug::Log(status_color3, "Network: Server has disconnected!");
	}
	break;
	}
}