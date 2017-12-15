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
	case AVATAR_POS:
		++message;
		SetAvatarTransform(message);
		break;
	case ADD_HAZARD:
		AddHazard();
		break;
	case HAZARD_POS:
		++message;
		SetHazardTransform(message);
		break;
	case ADD_SECONDARY_AVATAR:
		++message;
		AddSecondaryAvatar(message);
		break;
	case SEC_AVATAR_UPDATE:
		++message;
		SetSecondaryAvatarTransform(message);
		break;
	case NAV_MESH:
		++message;
		HandleNavMesh(message);
		break;
	case AVATAR_VEL_UPDATE:
		++message;
		HandleAvatarVelocity(message);
	case SEC_AVATAR_VEL_UPDATE:
		++message;
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

	char* mazeData = reinterpret_cast<char*>(message);
	std::string s(mazeData);
	std::istringstream ss(mazeData);
	SAFE_DELETE(mazeGenerator);
	mazeGenerator = new MazeGenerator();
	// Generate an empty maze
	mazeGenerator->Generate(mazeDim, 0.0f);
	mazeGenerator->Deserialize(ss);
	std::cout << "Client has received maze parameters" << std::endl;

	RefreshMazeRenderer();
}

void MazeClient::SendRouteRequest(GraphNode* const start, GraphNode* const end) {
	SendRouteRequest(mazeGenerator->GetIndexFromNode(start), mazeGenerator->GetIndexFromNode(end));
}

void MazeClient::SendRouteRequest(int startIdx, int endIdx) {
	std::cout << "Sending route request to server" << std::endl;
	std::ostringstream ss;
	ss << startIdx << std::endl;
	ss << endIdx << std::endl;
	PacketString routeRequest(ROUTE_REQUEST, ss.str());
	ENetPacket* packet = enet_packet_create(&routeRequest, sizeof(routeRequest), 0);
	enet_peer_send(serverConnection, 0, packet);
}

void MazeClient::HandleMazeRoute(Packets::PacketType* message) {
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

void MazeClient::HandleNavMesh(Packets::PacketType* message) {
	char* route = reinterpret_cast<char*>(message);
	std::string s = std::string(route);
	std::istringstream ss(s);
	std::vector<int> routeIndices;
	int index;
	while (ss >> index) {
		routeIndices.push_back(index);
	}
	std::cout << "Received maze route from server" << std::endl;
	navMesh = MakePathFromIndices(routeIndices);
}

void MazeClient::CreateAvatar() {

	avatar = MakeAvatarRenderNode();

	if (mazeRenderer) {
		mazeRenderer->Render()->AddChild(avatar);
	}

	PacketCharArray avatarPacket(CREATE_AVATAR);
	std::ostringstream ss;
	// Start/End Points
	ss << mazeGenerator->GetStartIndex() << std::endl;
	ss << mazeGenerator->GetGoalIndex() << std::endl;
	memcpy(avatarPacket.data, ss.str().c_str(), ss.str().size() * sizeof(char));
	ENetPacket* packet = enet_packet_create(&avatarPacket, sizeof(avatarPacket), 0);
	enet_peer_send(serverConnection, 0, packet);
	std::cout << "Sent request to create avatar" << std::endl;
}

void MazeClient::HandleKeyboardInput(KeyboardKeys key) {
	switch (key) {
	case KEYBOARD_G:
		SendMazeParams(true);
		break;
	case KEYBOARD_R:
		showRoute = !showRoute;
		break;
	case KEYBOARD_N:
		showNavMesh = !showNavMesh;
		break;
	case KEYBOARD_H:
		SendHazardRequest();
		break;
	case KEYBOARD_P:
		clientPrediction = !clientPrediction;
		break;
	case KEYBOARD_1:
		if (mazeDim > 3) {
			--mazeDim;
			SendMazeParams(true);
		}
		break;
	case KEYBOARD_2:
		if (mazeDim < 15) {
			++mazeDim;
			SendMazeParams(true);
		}
		break;
	case KEYBOARD_3:
		if (mazeDensity > 0.049f) {
			mazeDensity -= 0.05f;
			SendMazeParams(true);
		}
		break;
	case KEYBOARD_4:
		if (mazeDensity < 0.9501f) {
			mazeDensity += 0.05f;
			SendMazeParams(true);
		}
		break;
	default:
		break;
	}
}

void MazeClient::HandleMouseInput(MouseButtons button) {
	switch (button) {
	case MOUSE_LEFT:
		//ChangeStartPoint(rand() % 30);
		break;
	case MOUSE_RIGHT:
		//ChangeEndPoint(rand() % 30);
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
	if (showRoute && !path.empty()) {
		DrawPath();
	}
	if (showNavMesh) {
		DrawNavMesh();
	}

	if (clientPrediction && avatar) {
		timeSinceLastPosUpdate += dt;
		UpdateAvatar(timeSinceLastPosUpdate);
	}
}

void MazeClient::RegisterMazeWithScreenPicker() {
	GraphNode* node = mazeGenerator->GetAllNodesArr();

	// Add a transparent cube object with callback to each node
	for (int i = 0; i < mazeDim * mazeDim; ++i) {

		Vector3 cellpos = Vector3(
			(node + i)->_pos.x,
			0.0f,
			(node + i)->_pos.y
		);
		
		RenderNode* cube = new RenderNode(nodeClickMesh, Vector4(0.0f, 1.0f, 0.0f, 0.0f));
		clickerCubes.push_back(cube);
		cube->SetTransform(MazeSpaceToWorldSpace(cellpos));

		mazeRenderer->Render()->AddChild(cube);

		using namespace placeholders;
		ScreenPicker::Instance()->RegisterNodeForMouseCallback(cube, std::bind(&MazeClient::NodeSelectedCallback, this, cube, i,  _1, _2, _3, _4));
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


void MazeClient::NodeSelectedCallback(RenderNode* obj, int idx, float dt, const Vector3& newWsPos, const Vector3& wsMovedAmount, bool stopDragging)
{
		if (Window::GetMouse()->ButtonDown(MOUSE_LEFT))
		{
			ChangeStartPoint(idx);
		}
		else if (Window::GetMouse()->ButtonDown(MOUSE_RIGHT))
		{
			ChangeEndPoint(idx);
		}
}


void MazeClient::ChangeStartPoint(int newIndex) {
	std::cout << "Changing start point " << std::endl;
	mazeGenerator->SetStartIndex(newIndex);

	RefreshMazeRenderer(false);

	SendRouteRequest(mazeGenerator->GetStartIndex(), mazeGenerator->GetGoalIndex());
}

void MazeClient::ChangeEndPoint(int newIndex) {
	std::cout << "Changing end point " << std::endl;
	mazeGenerator->SetGoalIndex(newIndex);

	RefreshMazeRenderer(false);

	SendRouteRequest(mazeGenerator->GetStartIndex(), mazeGenerator->GetGoalIndex());
}

void MazeClient::SetAvatarTransform(Packets::PacketType* message) {
	if (!avatar) {
		return;
	}
	avatarPosition = *reinterpret_cast<Vector3*>(message);
	timeSinceLastPosUpdate = 0.0f;
	avatar->SetTransform(MazeSpaceToWorldSpace(avatarPosition));
}

void MazeClient::DrawNavMesh() {
	mazeRenderer->DrawPath(navMesh, true, 0.3f, 0.3f);
}

void MazeClient::RefreshMazeRenderer(bool registerClick) {
	UnregisterMazeWithScreenPicker();


	this->RemoveGameObject(mazeRenderer, true);

	SAFE_DELETE(mazeRenderer);
	mazeRenderer = new MazeRenderer(mazeGenerator);
	mazeRenderer->Render()->SetTransform(Matrix4::Scale(mazeRenderScale));
	//TODO: Re add Hazards
	this->AddGameObject(mazeRenderer);
	CreateAvatar();
	// Re-add secondary avatars
	for (auto& avatar : secondaryAvatars) {
		avatar.second = MakeAvatarRenderNode(Vector4(0.3f, 1.0f, 0.0f, 1.0f));
		mazeRenderer->Render()->AddChild(avatar.second);
	}

	RegisterMazeWithScreenPicker();

}


void MazeClient::AddHazard() {
	hazards.push_back(CommonUtils::BuildCuboidObject(
		"Hazard" + std::to_string(numHazards),
		Vector3(0.0f),
		0.3f,
		false,
		0.0f,
		false,
		false,
		Vector4(1.0f, 0.2f, 0.2f, 1.0f)
	));
	hazards[numHazards]->Render()->SetTransform(Matrix4::Scale(Vector3(0.1f, 1.5f, 0.1f)));
	mazeRenderer->Render()->AddChild(hazards[numHazards++]->Render());
}

void MazeClient::SendHazardRequest() {
	Packet hazard(ADD_HAZARD_REQUEST);
	ENetPacket* packet = enet_packet_create(&hazard, sizeof(hazard), 0);
	enet_peer_send(serverConnection, 0, packet);
}


void MazeClient::SetHazardTransform(Packets::PacketType* message) {
	int* hazardID = reinterpret_cast<int*>(message);
	++message;
	Vector3* pos = reinterpret_cast<Vector3*>(message);

	hazards[*hazardID]->Render()->SetTransform(MazeSpaceToWorldSpace(*pos));
}

void MazeClient::UnregisterMazeWithScreenPicker() {
	for (auto cube : clickerCubes) {
		ScreenPicker::Instance()->UnregisterNodeForMouseCallback(cube);
	}
}

void MazeClient::AddSecondaryAvatar(Packets::PacketType* message) {
	int* ID = reinterpret_cast<int*>(message);
	secondaryAvatars[*ID] = MakeAvatarRenderNode(Vector4(0.3f, 1.0f, 0.0f, 1.0f));
}

RenderNode* MazeClient::MakeAvatarRenderNode(Vector4 colour) {
	RenderNode* rNode = new RenderNode();

	RenderNode* dummy = new RenderNode(CommonMeshes::Sphere(), colour);
	dummy->SetTransform(Matrix4::Scale(avatarScale));
	rNode->AddChild(dummy);

	rNode->SetBoundingRadius(0.4f);
	return rNode;
}

void MazeClient::SetSecondaryAvatarTransform(Packets::PacketType* message) {
	int* ID = reinterpret_cast<int*>(message);
	++message;
	Vector3* pos = reinterpret_cast<Vector3*>(message);
	secondaryAvatars[*ID]->SetTransform(MazeSpaceToWorldSpace(*pos));
}

Matrix4 MazeClient::MazeSpaceToWorldSpace(Vector3 pos) {
	const float grid_scalar = 1.0f / (float)mazeGenerator->GetSize();
	const float scalar = 1.f / (float)mazeRenderer->GetFlatMazeSize();

	Vector3 cellpos = Vector3(
		pos.x * 3,
		0.0f,
		pos.z * 3
	) * scalar;

	// Could precompute this every time generated new maze to save a few multiplies
	Vector3 cellSize = Vector3(
		scalar * 2,
		1.0f,
		scalar * 2
	);

	return Matrix4::Translation(cellpos + cellSize * 0.5f) * Matrix4::Scale(cellSize * 0.5f);
}

void MazeClient::HandleAvatarVelocity(Packets::PacketType* message) {
	avatarVelocity = *reinterpret_cast<Vector3*>(message);
}

void MazeClient::UpdateAvatar(float time) {
	avatar->SetTransform(MazeSpaceToWorldSpace(avatarPosition + avatarVelocity * time));
}
//std::istringstream ss(*reinterpret_cast<std::string*>(message));
//std::vector<int> routeIndices;
//int index;
//while (ss >> index) {
//	routeIndices.push_back(index);
//}

//MyPacket header;
//ENetPacket* packet = enet_packet_create(&header, sizeof(header) + sizeof(MyPacketNodeData) * 100, 0);

//MyPacket* header;
//header = reinterpret_cast<MyPacket*>(packetData); //memcpy this also otherwise when enet packet goers out of scope will delete mry|
//MyPacketNodeData* tmpArr = new MyPacketNodeData[header->numNodes];
//memcpy(tmpArr, reinterpret_cast<char*>(packetData) + sizeof(MyPacket), sizeof(MyPacketNodeData) * header->numNodes);

//enet_host_broadcast(serverNetwork.m_pNetwork, 0, packet);
//avatar->Render()->SetTransform(Matrix4::Translation(mazeRenderer->GetCellSize() * 0.5f) *
//	Matrix4::Scale(mazeRenderer->GetCellSize() * 0.5f));
//this->AddGameObject(avatar);

//TODO: A little inefficient
//SAFE_DELETE(mazeRenderer);
//mazeRenderer = new MazeRenderer(mazeGenerator);
//mazeRenderer->Render()->SetTransform(Matrix4::Scale(Vector3(10.0f, 1.0f, 10.0f))); //TODO: Define this scale factor
//GraphNode startEnd[2] = { *start, *end };
//PacketNode endPoints(ROUTE_REQUEST, startEnd);
//
//ENetPacket* packet = enet_packet_create(&endPoints, sizeof(PacketType) + sizeof(GraphNode) * 2, 0);
//enet_peer_send(serverConnection, 0, packet);