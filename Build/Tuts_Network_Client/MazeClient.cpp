#include "MazeClient.h"

#include <ncltech\Packets.h>
using namespace Packets;

#include <ncltech\MazeGenerator.h>
#include <ncltech\MazeRenderer.h>

#include <ncltech\CommonUtils.h>

MazeClient::MazeClient(std::string name) :
	Net1_Client(name), avatar(nullptr)
{
}


MazeClient::~MazeClient()
{
}

void MazeClient::OnConnection() {
	Net1_Client::OnConnection();

	// Send server maze parameters
	SendMazeParams();
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
	//TeMP
	mazeGenerator = new MazeGenerator();
	int* seed = reinterpret_cast<int*>(message);
	mazeGenerator->Generate(*seed, mazeDim, mazeDensity);
	//End Temp
	if (!mazeGenerator) {
		std::string* mazeData = reinterpret_cast<std::string*>(message);
		std::stringstream ss(*mazeData, std::stringstream::in);
		mazeGenerator = new MazeGenerator();
		mazeGenerator->Deserialize(ss);
		std::cout << "Client has received maze parameters" << std::endl;
	}

	mazeRenderer = new MazeRenderer(mazeGenerator);
	mazeRenderer->Render()->SetTransform(Matrix4::Scale(Vector3(10.0f, 1.0f, 10.0f)));
	this->AddGameObject(mazeRenderer);

	avatar = CommonUtils::BuildSphereObject(
		"avatar",
		mazeGenerator->GetStartNode()->GetPos(),
		0.6f,
		false,
		0.0f,
		false,
		false,
		Vector4(1.0f, 0.0f, 1.0f, 1.0f)
	);
	this->AddGameObject(avatar);
}

void MazeClient::HandleKeyboardInput(KeyboardKeys key) {
	switch (key) {
	case KEYBOARD_G:
		SendMazeParams(true);
	default:
		break;
	}
}

void MazeClient::HandleMouseInput(MouseButtons button) {
	switch (button) {
	case MOUSE_LEFT:
		break;
	case MOUSE_RIGHT:
		break;
	}
}

void MazeClient::SendMazeParams(bool resend) {
	PacketIntFloat mazeParams;
	if (resend) {
		std::cout << "Sending request to regenerate maze" << std::endl;
		mazeParams = PacketIntFloat(REGEN_MAZE, mazeDim, mazeDensity);
	}
	else {
		std::cout << "Sending maze parameters" << std::endl;
		mazeParams= PacketIntFloat(MAZE_PARAM, mazeDim, mazeDensity);
	}

	ENetPacket* packet = enet_packet_create(&mazeParams, sizeof(mazeParams), 0);
	enet_peer_send(serverConnection, 0, packet);
}

void MazeClient::OnUpdateScene(float dt) {
	Net1_Client::OnUpdateScene(dt);
}