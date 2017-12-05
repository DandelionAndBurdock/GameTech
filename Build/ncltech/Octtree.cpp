#include "Octtree.h"

#include <set>

#include "PhysicsEngine.h"

Hull Octtree::cubeHull = Hull();

Octtree::Octtree(Vector3 minCorner, Vector3 maxCorner) 
{
	root = new OcttreeNode(minCorner, maxCorner);

	if (cubeHull.GetNumVertices() == 0){
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
	if (!object->GetBroadCollisionShape()) {
		return;
	}
	if (!node->region->IsSphereInCube(object->GetPosition(), object->GetBroadCollisionShape()->GetSize())) {
		return;
	}
	// If node has children find correct child
	if (node->hasChildren) {
		for (int i = 0; i < NUM_OCTANTS; ++i) {
			InsertObject(object, node->children[i]);
		}
	}
	else { // If no children add to this node
		node->objects.push_back(object);
	}

	// If too many objects try and divide
	if (node->objects.size() > maxOctantObjects) {
			Divide(node);
			// Reinsert objects into children 
			SendObjectsToChildren(node);	
	}
}


void Octtree::DebugDraw() {
	DebugDraw(root);
}

void Octtree::SendObjectsToChildren(OcttreeNode* node) {
	// Should not do anything if node has no children
	if (!node->hasChildren) {
		return;
	}

	for (int i = 0; i < NUM_OCTANTS; ++i) {
		for (auto& object : node->objects) { // This looks inefficient but number of objects should never be too large
			InsertObject(object, node->children[i]);
		}
	}
	// Erase objects in parent
	node->objects.clear();
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


void Octtree::Update() {

	UpdateNonStaticObjects(root); 
	
	UpdateRecombine(root);

	UpdateDivision(root);
}



void Octtree::UpdateNonStaticObjects(OcttreeNode* node) {
	if (node->hasChildren) { // Only need to check the nodes at the end of the tree
		for (int i = 0; i < NUM_OCTANTS; ++i) {
			UpdateNonStaticObjects(node->children[i]);
		}
	}
	else { 
		// If have a lot of objects that will never move e.g. buildings would benefit performance to maintain a seperate list or test for STATIONARY flag
		for (auto iter = node->objects.begin(); iter != node->objects.end(); ) {
				if (!node->region->IsSphereInCube((*iter)->GetPosition(), (*iter)->GetBroadCollisionShape()->GetSize())) { 

					// Reinsert without divide
					SimpleInsertObject(*iter, root);

					// Erase object from this list
					iter = node->objects.erase(iter);
				}
				else {
					++iter;
				}
		}
		
	}
}
			  
void Octtree::UpdateDivision(OcttreeNode* node) {
	if (node->hasChildren) { 
		for (int i = 0; i < NUM_OCTANTS; ++i) {
			UpdateDivision(node->children[i]);
		}
	}
	else { // Node has not been divided
		if (node->objects.size() > maxOctantObjects) {
			Divide(node);
			// Reinsert objects into children 
			SendObjectsToChildren(node);
		}
	}
}

void Octtree::UpdateRecombine(OcttreeNode* node) {
	if (!node->hasChildren) { // Can't recombine undivided node
		return;
	}

	for (int i = 0; i < NUM_OCTANTS; ++i) { // Recombine from bottom up
		UpdateRecombine(node->children[i]);
	}

	if (node != root && CountChildren(node) < maxOctantObjects) {
		CountChildren(node);
		// Reabsorb the children objects
		for (int i = 0; i < NUM_OCTANTS; ++i) {
			node->objects.insert(node->objects.end(),
								 node->children[i]->objects.begin(), 
								 node->children[i]->objects.end());
		}
		
		// Delete the children
		for (int i = 0; i < NUM_OCTANTS; ++i) {
			SAFE_DELETE(node->children[i]);
		}
		node->hasChildren = false;
	}
}


int Octtree::CountChildren(OcttreeNode* node) {
	if (!node->hasChildren) { 
		return 0;
	}

	std::set<PhysicsNode*> uniqueObjects; 	// Use a set so that don't count same boundary objects twice
	GetObjects(uniqueObjects, node);

	return uniqueObjects.size();
}

void Octtree::GetObjects(std::set<PhysicsNode*> s, OcttreeNode* node) {
	if (node->hasChildren) {
		for (int i = 0; i < NUM_OCTANTS; ++i) {
			GetObjects(s, node->children[i]);
		}
	}
	else {
		s.insert(
			node->objects.begin(),
			node->objects.end());
	}


}

void Octtree::SimpleInsertObject(PhysicsNode* object, OcttreeNode* node) {
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
			SimpleInsertObject(object, node->children[i]);
		}
	}
	else { // If no children add to this node
		node->objects.push_back(object);
	}
}

bool Octtree::AtMinimumSize(OcttreeNode* node) {
	Vector3 nodeSize = node->GetSize();
	if (nodeSize.x <= minCubeSize) {
		return true;
	}
	if (nodeSize.y <= minCubeSize) {
		return true;
	}
	if (nodeSize.z <= minCubeSize) {
		return true;
	}

	return false;
}


std::vector<CollisionPair*> Octtree::BuildPotentialCollisionList() {
	for (auto& pair : pairList) { // Clear old pair list
		SAFE_DELETE(pair);
	}

	BuildPotentialCollisionList(root, pairList);
	return pairList;
}

void Octtree::BuildPotentialCollisionList(OcttreeNode* node, std::vector<CollisionPair*>& pairList) {
	if (node->hasChildren) { 
		for (int i = 0; i < NUM_OCTANTS; ++i) {
			BuildPotentialCollisionList(node->children[i], pairList);
		}
	}

	if (node->objects.size() > 1) {
		for (int firstObj = 0; firstObj < node->objects.size(); ++firstObj) {
			for (int secondObj = 0; secondObj < node->objects.size(); ++secondObj) {
				pairList.push_back(new CollisionPair(node->objects[firstObj], node->objects[secondObj]));
			}
		}
	}
}

