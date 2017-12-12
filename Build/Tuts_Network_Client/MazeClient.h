#pragma once

#include "Net1_Client.h"
#include <ncltech\Packets.h>

class MazeGenerator;
class MazeRenderer;
class GraphNode;

class MazeClient : public Net1_Client
{
public:
	MazeClient(std::string name);
	~MazeClient();

protected:
	void HandlePosData(Packets::PacketType* message);
	void OnConnection() override;

	void ReceiveMessage(const ENetEvent& evnt) override;

	void HandleMazeStructure(Packets::PacketType* message);

	void HandleKeyboardInput(KeyboardKeys key) override;
	void HandleMouseInput(MouseButtons button) override;

	void OnUpdateScene(float dt) override;
protected:
	// Send maze params to the server
	void SendMazeParams(bool resend = false);

	void CreateAvatar();

	// Make maze nodes clickable
	void RegisterMazeWithScreenPicker();

	// Draw path between current start and end points
	void DrawPath();

	void SendRouteRequest(GraphNode* const start, GraphNode* const end);

	MazeGenerator* mazeGenerator = nullptr;
	MazeRenderer* mazeRenderer = nullptr;

	int mazeDim = 6;
	float mazeDensity = 0.5f;


	GameObject* avatar;
};

