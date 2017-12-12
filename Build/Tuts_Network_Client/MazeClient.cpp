#include "MazeClient.h"

#include <ncltech\Packets.h>
using namespace Packets;

#include <ncltech\MazeGenerator.h>
#include <ncltech\MazeRenderer.h>

#include <ncltech\CommonUtils.h>
#include <ncltech\ScreenPicker.h>

MazeClient::MazeClient(std::string name) :
	Net1_Client(name), avatar(nullptr)
{
	nodeClickMesh = CommonMeshes::Cube();
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
	case MAZE_ROUTE:
		++message;
		HandleMazeRoute(message);
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

	CreateAvatar();
	SendRouteRequest(mazeGenerator->GetStartNode(), mazeGenerator->GetGoalNode());
	RegisterMazeWithScreenPicker();
}

void MazeClient::SendRouteRequest(GraphNode* const start, GraphNode* const end) {
	//GraphNode startEnd[2] = { *start, *end };
	//PacketNode endPoints(ROUTE_REQUEST, startEnd);
	//
	//ENetPacket* packet = enet_packet_create(&endPoints, sizeof(PacketType) + sizeof(GraphNode) * 2, 0);
	//enet_peer_send(serverConnection, 0, packet);
	std::cout << "Sending route request to server" << std::endl;
	std::ostringstream ss; 
	ss << mazeGenerator->GetIndexFromNode(start) << std::endl;
	ss << mazeGenerator->GetIndexFromNode(end) << std::endl;
	PacketString routeRequest(ROUTE_REQUEST, ss.str());
	ENetPacket* packet = enet_packet_create(&routeRequest, sizeof(routeRequest), 0);
	enet_peer_send(serverConnection, 0, packet);
}


void MazeClient::HandleMazeRoute(Packets::PacketType* message) {
	//std::istringstream ss(*reinterpret_cast<std::string*>(message));
	//std::vector<int> routeIndices;
	//int index;
	//while (ss >> index) {
	//	routeIndices.push_back(index);
	//}
	char* route = reinterpret_cast<char*>(message);
	std::string s = std::string(route);
	std::istringstream ss(s);
	std::vector<int> routeIndices;
	int index;
	while (ss >> index) {
		routeIndices.push_back(index);
	}
	std::cout << "Received maze route from server" << std::endl;
	path = MakePathFromIndices(routeIndices);
}

void MazeClient::CreateAvatar() {

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
	mazeRenderer->Render()->AddChild(avatar->Render());
	avatar->Render()->SetTransform(Matrix4::Translation(mazeRenderer->GetCellSize() * 0.5f) *
		Matrix4::Scale(mazeRenderer->GetCellSize() * 0.5f));
	this->AddGameObject(avatar);
}

void MazeClient::HandleKeyboardInput(KeyboardKeys key) {
	switch (key) {
	case KEYBOARD_G:
		SendMazeParams(true);
	case KEYBOARD_M:
		//avatarPathIndex
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
		mazeParams = PacketIntFloat(MAZE_PARAM, mazeDim, mazeDensity);
	}

	ENetPacket* packet = enet_packet_create(&mazeParams, sizeof(mazeParams), 0);
	enet_peer_send(serverConnection, 0, packet);
}

void MazeClient::OnUpdateScene(float dt) {
	Net1_Client::OnUpdateScene(dt);
	if (!path.empty()) {
		DrawPath();
	}
	if (avatar) {
		UpdateAvatar();
	}

}

void MazeClient::RegisterMazeWithScreenPicker() {
	const float grid_scalar = 1.0f / (float)mazeGenerator->GetSize();
	const float scalar = 1.f / (float)mazeRenderer->GetFlatMazeSize();

	Vector3 cellSize = Vector3(
		scalar * 2,
		1.0f,
		scalar * 2
	);


	GraphNode* node = mazeGenerator->GetAllNodesArr();

	// Add a transparent cube object with callback to each node
	for (int i = 0; i < mazeDim * mazeDim; ++i) {
		Vector3 cellpos = Vector3(
			(node + i)->_pos.x * 3,
			0.0f,
			(node + i)->_pos.y * 3
		) * scalar;

		RenderNode* cube = new RenderNode(nodeClickMesh, Vector4(0.0f, 1.0f, 0.0f, 0.5f));
		cube->SetTransform(Matrix4::Translation(cellpos + cellSize * 0.5f) * Matrix4::Scale(cellSize * 0.5f));
		mazeRenderer->Render()->AddChild(cube);

		using namespace placeholders;
		//ScreenPicker::Instance()->RegisterNodeForMouseCallback(cube->Render(), std::bind(&MazeClient::NodeSelectedCallback, this, _1, _2, _3, _4, _5));
	}
}

void MazeClient::DrawPath() {
	mazeRenderer->DrawPath(path);
}

// Helper function: Makes a list of edges joining nodes together TODO: Move somewhere else
std::vector<GraphEdge> MazeClient::MakePathFromIndices(std::vector<int>& nodeIndices) {
	std::vector<GraphEdge> path;
	for (uint i = 0; i < nodeIndices.size() - 1; ++i){
		GraphEdge edge;
		edge._a = mazeGenerator->GetNodeFromIndex(nodeIndices[i]);
		edge._b = mazeGenerator->GetNodeFromIndex(nodeIndices[i + 1]);
		path.push_back(edge);
	}
	return path;
}



//MyPacket header;
//ENetPacket* packet = enet_packet_create(&header, sizeof(header) + sizeof(MyPacketNodeData) * 100, 0);

//MyPacket* header;
//header = reinterpret_cast<MyPacket*>(packetData); //memcpy this also otherwise when enet packet goers out of scope will delete mry|
//MyPacketNodeData* tmpArr = new MyPacketNodeData[header->numNodes];
//memcpy(tmpArr, reinterpret_cast<char*>(packetData) + sizeof(MyPacket), sizeof(MyPacketNodeData) * header->numNodes);


void MazeClient::UpdateAvatar() {
}

void MazeClient::NodeSelectedCallback(GameObject* obj, float dt, const Vector3& newWsPos, const Vector3& wsMovedAmount, bool stopDragging)
{
	// Get index from name
	int index;
	std::string name = obj->GetName();
	auto iter = name.find_first_of(':');
	if (iter = std::string::npos) {
		return;
	}
	else {
		++iter;
		index = std::stoi(name.substr(iter));
	}
		if (Window::GetMouse()->ButtonDown(MOUSE_LEFT))
		{
			std::cout << "Left click" << index << std::endl;
		}
		else if (Window::GetMouse()->ButtonDown(MOUSE_RIGHT))
		{
			std::cout << "Right click" << index << std::endl;
		}
}