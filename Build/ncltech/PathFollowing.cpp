#include "PathFollowing.h"

#include "GameObject.h"
#include "CommonUtils.h"
using namespace CommonUtils;

const float time_between_nodes = 0.5f;
const int num_control_points = 10;
const float total_circuit_time = float(num_control_points) * time_between_nodes;

PathFollowing::PathFollowing(GameObject* follower)
{
	elapsed_time = 0.0f;
	cubic_tangent_weighting = 0.5f;

 // Not brilliant randomness but okay if only make a few paths
	float radius = float(rand() % 5) + 15.0f;
	float heightMin = float(rand() % 2) + 2.0f;
	float heightMax = float(rand() % 2) + 4.0f;
	static bool reverse = false;
	reverse = !reverse;

	//Create some balls to drag around and form our path
	const Vector4 ball_col = Vector4(1.0f, 0.f, 0.f, 1.0f);
	GameObject* obj;
	for (int i = 0; i < num_control_points; ++i)
	{
		float angle = (i / (float)num_control_points) * 2.0f * PI;
		float x;
		float z;
		if (reverse) {
			x = cosf(angle) * radius;
			z = sinf(angle) * radius;
		}
		else {
			x = cosf(-angle) * radius;
			z = sinf(-angle) * radius;
		}

		obj = CommonUtils::BuildSphereObject(
			"",
			Vector3(x, (i % 3 == 0) ? heightMax : heightMin, z),
			0.3f,
			false,
			0.0f,
			false,
			true,
			ball_col);
		path_nodes.push_back(obj);
	}

	player = follower;
}


PathFollowing::~PathFollowing()
{
}

void PathFollowing::GetNodeIdxAndFactor(float elapsed, int& out_idx, float& out_factor)
{
	elapsed = fmod(elapsed, total_circuit_time);

	//Get the current node in elapsed time 
	float node_time = elapsed / time_between_nodes;

	//Get the current node index - floor(node_time)
	// and the factor - decimal part
	float node_time_floored;

	out_factor = modf(node_time, &node_time_floored);
	out_idx = (int)node_time_floored;
}

void PathFollowing::Update(float dt)
{
	//Build rotations for path based on their positions
	UpdatePathForwardVectors();


	const uint num_nodes = path_nodes.size();
	elapsed_time += dt;
	elapsed_time = fmod(elapsed_time, total_circuit_time);


	int current_node;
	float factor;
	GetNodeIdxAndFactor(elapsed_time, current_node, factor);

	//Position
	Vector3 player_pos;
	//Cubic/Hermite spline
	//Requires nodes previous and ahead of current segment
	player_pos = InterpolatePositionCubic(
		(current_node + num_nodes - 1) % num_nodes,
		current_node,
		(current_node + 1) % num_nodes,
		(current_node + 2) % num_nodes,
		factor);

	//Rotation
	Quaternion player_rot;

	// - Fast and simple.. the better the positional interpolation, the better the rotation =]
	GetNodeIdxAndFactor(elapsed_time + dt, current_node, factor);
	Vector3 ahead;
	ahead = InterpolatePositionCubic(
		(current_node + num_nodes - 1) % num_nodes,
		current_node,
		(current_node + 1) % num_nodes,
		(current_node + 2) % num_nodes,
		factor);
	player_rot = Quaternion::LookAt(player_pos, ahead);

	player->Physics()->SetPosition(player_pos);
	player->Physics()->SetOrientation(player_rot);
	//player->Render()->SetTransform(Matrix4::Translation(player_pos) * player_rot.ToMatrix4());

}

//Cubicly interpolates between b-c (with a and d being the nodes after/before the current segment)
Vector3 PathFollowing::InterpolatePositionCubic(int node_a, int node_b, int node_c, int node_d, float f)
{
	//With factor between 0-1, this is defined as:
	// - See wiki Hermite Spline
	Vector3 posA = path_nodes[node_a]->Render()->GetTransform().GetPositionVector();
	Vector3 posB = path_nodes[node_b]->Render()->GetTransform().GetPositionVector();
	Vector3 posC = path_nodes[node_c]->Render()->GetTransform().GetPositionVector();
	Vector3 posD = path_nodes[node_d]->Render()->GetTransform().GetPositionVector();

	//Get the tangents at points B and C
	// - These are the exit velocities of the two points, so if you change the 0.5f factor
	//   here you will either expand or shrink the amount of curvature. The half default
	//   means that the direction of the curve entering/leaving will be half way towards the
	//   the direction needed to hit the next point.
	Vector3 tanB = (posC - posA) * cubic_tangent_weighting;
	Vector3 tanC = (posD - posB) * cubic_tangent_weighting;

	float f2 = f * f;
	float f3 = f2 * f;

	return posB * (f3 * 2.0f - 3.0f * f2 + 1.0f)
		+ tanB * (f3 - 2.0f * f2 + f)
		+ posC * (-2.0f * f3 + 3.0f * f2)
		+ tanC * (f3 - f2);
}


void PathFollowing::UpdatePathForwardVectors()
{
	//To do rotational interpolation we need to generate some rotations for the path nodes.
	// - For the node example we can just do a LookAt from each control point to the next
	//   node in the path.
	const uint size = path_nodes.size();

	Vector3 centre = Vector3(0, 0, 0);
	for (uint i = 0; i < size; ++i)
	{
		centre = centre + path_nodes[i]->Render()->GetTransform().GetPositionVector();
	}
	centre = centre / (float)size;

	path_rotations.resize(size);
	for (uint i = 0; i < size; ++i)
	{
		Vector3 cur_node_pos = path_nodes[i % size]->Render()->GetTransform().GetPositionVector();
		Vector3 nxt_node_pos = path_nodes[(i + 1) % size]->Render()->GetTransform().GetPositionVector();

		path_rotations[i] = Quaternion::LookAt(cur_node_pos, nxt_node_pos, Vector3(0, 1, 0));
	}
}