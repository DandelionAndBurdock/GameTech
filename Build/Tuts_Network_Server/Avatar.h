#pragma once
#include <enet\enet.h>
#include <list>
#include <vector>

#include <nclgl\Vector2.h>
#include <nclgl\Vector3.h>

#include <ncltech\MazeGenerator.h>

class PhysicsNode;


class Avatar
{
public:
	Avatar(ENetPeer* client);
	~Avatar();


	ENetPeer* GetClient() { return client; }


	void SetPath(const std::list<const GraphNode*> path);

	Vector3 GetPosition();
	Vector3 GetVelocity();
	PhysicsNode* GetPhysics() { return physics; }
protected:
	PhysicsNode* physics = nullptr;

	// Client this avatar lives on
	ENetPeer* client;


	// List of nodes on the path
	std::vector<const GraphNode*> path;
	// Current index in path
	int currentPathIndex;
};