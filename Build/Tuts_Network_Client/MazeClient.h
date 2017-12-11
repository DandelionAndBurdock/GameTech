#pragma once

#include "Net1_Client.h"
#include <ncltech\Packets.h>

class MazeGenerator;
class MazeRenderer;

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
	MazeGenerator* mazeGenerator = nullptr;
	MazeRenderer* mazeRenderer = nullptr;

	int mazeDim = 6;
	float mazeDensity = 0.5f;


	GameObject* avatar;
};

