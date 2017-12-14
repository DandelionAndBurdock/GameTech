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
	void HandleMazeRoute(Packets::PacketType* message);
	void AddHazard();

	void HandleKeyboardInput(KeyboardKeys key) override;
	void HandleMouseInput(MouseButtons button) override;

	void OnUpdateScene(float dt) override;
protected:
	// Send maze params to the server
	void SendMazeParams(bool resend = false);

	void CreateAvatar();
	void SetAvatarTransform(Packets::PacketType* message);
	void SetHazardTransform(Packets::PacketType* message);
	// Make maze nodes clickable
	void RegisterMazeWithScreenPicker();
	void UnregisterMazeWithScreenPicker();
	std::vector<RenderNode*> clickerCubes;
	// Draw path between current start and end points
	void DrawPath();

	void ChangeStartPoint(int index);
	void ChangeEndPoint(int index);
	void SendRouteRequest(GraphNode* const start, GraphNode* const end);
	void SendRouteRequest(int startIdx, int endIdx);
	void SendHazardRequest();

	void DrawNavMesh();

	MazeGenerator* mazeGenerator = nullptr;
	MazeRenderer* mazeRenderer = nullptr;

	int mazeDim = 10;
	float mazeDensity = 0.5f;


	RenderNode* avatar;
	int avatarPathIndex = 0;
	std::vector<GraphEdge> path;

	std::vector<GameObject*> hazards;
	int numHazards = 0;

	// Callback function which can be called when a node is clicked
	void NodeSelectedCallback(RenderNode* obj, int idx, float dt, const Vector3& newWsPos, const Vector3& wsMovedAmount, bool stopDragging);

	Vector3 mazeRenderScale = Vector3(10.0f, 1.0f, 10.0f);
	Vector3 avatarScale = Vector3(0.4f, 2.0f, 0.4f);

	bool showRoute = false;
	bool showNavMesh = false;

	void RefreshMazeRenderer(bool registerClick = true);

	// Key is the ID of the avatar which is sent from the server
	std::map<int, RenderNode*> secondaryAvatars;
	void AddSecondaryAvatar(Packets::PacketType* message);
	void SetSecondaryAvatarTransform(Packets::PacketType* message);

private:
	// Helper function: Makes a list of edges joining nodes together TODO: Move somewhere else
	std::vector<GraphEdge> MakePathFromIndices(std::vector<int>& nodeIndices);

	// Mesh defining where a node can be selected with mouse
	Mesh*			nodeClickMesh;

	// Helper function makes an avatar render node
	RenderNode* MakeAvatarRenderNode(Vector4 colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	// Helper function returns a transformation from maze space to world space
	Matrix4 MazeSpaceToWorldSpace(Vector3 pos);

};

