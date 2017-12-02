// Class to pull apart the bits of scene_pathFollowing that I need with some small modifications
#pragma once

class GameObject;

#include <vector>
#include <nclgl\Quaternion.h>
#include <nclgl\Vector3.h>
class PathFollowing
{
public:
	PathFollowing(GameObject* follower);
	~PathFollowing();

	void Update(float dt);
protected:
	GameObject* player;
	std::vector<GameObject*> path_nodes;
	std::vector<Quaternion> path_rotations; //These can matrices, or stored in the rendernodes themselves etc

	int current_node;
	float elapsed_time;

	float cubic_tangent_weighting;
	void GetNodeIdxAndFactor(float elapsed, int& out_idx, float& out_factor);
	Vector3 InterpolatePositionCubic(int node_a, int node_b, int node_c, int node_d, float f);
	void UpdatePathForwardVectors();

};







