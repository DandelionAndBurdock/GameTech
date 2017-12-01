#include "Octtree.h"

Hull Octtree::cubeHull = Hull();

Octtree::Octtree(Vector3 minCorner, Vector3 maxCorner) 
{
	root = new OcttreeNode(minCorner, maxCorner);
	//Divide(root);
	//Divide(root->children[0]);
	if (cubeHull.GetNumVertices() == 0)
	{
		ConstructCubeHull();
	}
}


Octtree::~Octtree()
{
	// Recursively delete all roots
}

//TODO: Remove object
void Octtree::InsertObject(PhysicsNode* node) {
	// Check if this object is already in the tree
	if(std::find(objects.cbegin(), objects.cend(), node) == objects.cend()) {
		objects.push_back(node);
	}

}

void Octtree::InsertObjects(std::vector<PhysicsNode*> objects) {
	for (auto& object : objects) {
		InsertObject(object);
	}
}

void Octtree::BuildTree() {
	for (auto& object : objects) {
		InsertObject(object, root);
	}
}


void Octtree::Divide(OcttreeNode* node) {
	if (node->hasChildren) { // Already divided this node
		return;
	}
	// Size of each new octant
	Vector3 divisionSize = node->GetSize() / 2.0f; // Divide each axis in half will give 8 octants
	// If cube size is too small stop division
	if (divisionSize.x < minCubeSize || divisionSize.y < minCubeSize || divisionSize.z < minCubeSize) {
		return;
	}
	// Starting minimum corner
	Vector3 startingMinCorner = node->GetMinCorner();

	// Min/Max Corner of current sub-octant
	Vector3 minCorner; 
	Vector3 maxCorner; 
	Vector3 offset;				// Holds offset from startingMinCorner to current minCorner
	int childIndex = 0;			
	// Split box into 8 octants
	for (int colX = 0; colX < 2; ++colX) {
		for (int colY = 0; colY < 2; ++colY) {
			for (int colZ = 0; colZ < 2; ++colZ) {
				offset = Vector3(colX * divisionSize.x, colY * divisionSize.y, colZ * divisionSize.z);
				minCorner = startingMinCorner + offset;
				maxCorner = minCorner + divisionSize;
				node->AddChild(new OcttreeNode(minCorner, maxCorner), childIndex++);
			}
		}
	}
}

// Implement remove object

void Octtree::InsertObject(PhysicsNode* object, OcttreeNode* node) {
	if (!node || !object) {
		return;
	}
	// Only add if object in bounds of this node
	if (!node->region->IsSphereInCube(object->GetPosition(), object->GetBroadCollisionShape()->GetSize())) {
		return;
	}
	// If node has children find correct child
	if (node->hasChildren) {
		for (int i = 0; i < NUM_OCTANTS; ++i) {
			InsertObject(object, node);
		}
	}
	else { // If no children add to this node
		node->objects.push_back(object);
	}

	// If too many objects try and divide
	if (node->objects.size() > maxOctantObjects) {
		Divide(node);
		// Reinsert objects into children 
		for (auto& object : node->objects) {
			for (int i = 0; i < NUM_OCTANTS; ++i) {
				InsertObject(object, node->children[i]);
			}
		}
		// Erase objects in parent
		objects.clear();
	}
}

void Octtree::DebugDraw() {
	DebugDraw(root);
}

void Octtree::DebugDraw(OcttreeNode* node) {
	Matrix4 transform = Matrix4::Translation(node->GetCentre()) * Matrix4::Scale(node->GetSize() / 2.0f);
	cubeHull.DebugDraw(transform);

	// Draw children (if they exist)
	for (int i = 0; i < NUM_OCTANTS; ++i) {
		if (node->children[i]) {
			DebugDraw(node->children[i]);
		}
	}
}

void Octtree::ConstructCubeHull()
{
	//Vertices
	cubeHull.AddVertex(Vector3(-1.0f, -1.0f, -1.0f));		// 0
	cubeHull.AddVertex(Vector3(-1.0f, 1.0f, -1.0f));		// 1
	cubeHull.AddVertex(Vector3(1.0f, 1.0f, -1.0f));		// 2
	cubeHull.AddVertex(Vector3(1.0f, -1.0f, -1.0f));		// 3

	cubeHull.AddVertex(Vector3(-1.0f, -1.0f, 1.0f));		// 4
	cubeHull.AddVertex(Vector3(-1.0f, 1.0f, 1.0f));		// 5
	cubeHull.AddVertex(Vector3(1.0f, 1.0f, 1.0f));		// 6
	cubeHull.AddVertex(Vector3(1.0f, -1.0f, 1.0f));		// 7

														//Indices ( MUST be provided in ccw winding order )
	int face1[] = { 0, 1, 2, 3 };
	int face2[] = { 7, 6, 5, 4 };
	int face3[] = { 5, 6, 2, 1 };
	int face4[] = { 0, 3, 7, 4 };
	int face5[] = { 6, 7, 3, 2 };
	int face6[] = { 4, 5, 1, 0 };

	//Faces
	cubeHull.AddFace(Vector3(0.0f, 0.0f, -1.0f), 4, face1);
	cubeHull.AddFace(Vector3(0.0f, 0.0f, 1.0f), 4, face2);
	cubeHull.AddFace(Vector3(0.0f, 1.0f, 0.0f), 4, face3);
	cubeHull.AddFace(Vector3(0.0f, -1.0f, 0.0f), 4, face4);
	cubeHull.AddFace(Vector3(1.0f, 0.0f, 0.0f), 4, face5);
	cubeHull.AddFace(Vector3(-1.0f, 0.0f, 0.0f), 4, face6);
}