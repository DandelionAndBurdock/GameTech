
#pragma once

#include <ncltech\Scene.h>
#include <ncltech\NetworkBase.h>

//Basic Network Example

class Net1_Client : public Scene
{
public:
	Net1_Client(const std::string& friendly_name);

	virtual void OnInitializeScene() override;
	virtual void OnCleanupScene() override;
	virtual void OnUpdateScene(float dt) override;


	virtual void ProcessNetworkEvent(const ENetEvent& evnt);

protected:
	// Fire any events on successful server connection
	virtual void OnConnection();

	virtual void ReceiveMessage(const ENetEvent& evnt);

	GameObject* box;

	NetworkBase network;
	ENetPeer*	serverConnection;
};