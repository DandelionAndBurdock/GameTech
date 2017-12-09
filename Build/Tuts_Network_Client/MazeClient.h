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


protected:
	MazeGenerator* mazeGenerator = nullptr;
	MazeRenderer* mazeRenderer = nullptr;
};

