#include "Octtree.h"

#include <set>

#include "PhysicsEngine.h"

#include "CommonUtils.h"

// Static variables
bool Octtree::debugDraw = true;

Octtree::Octtree(Vector3 minCorner, Vector3 maxCorner) 
{
	root = new OcttreeNode(minCorner, maxCorner);
}


Octtree::~Octtree()
{
	DeleteTree(root);
}


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
	// Clear any previous tree but leave root intact
	if (root->hasChildren) {
		for (int i = 0; i < NUM_OCTANTS; ++i) {
			DeleteTree(root->children[i]);
		}
	}
	else {
		root->objects.clear();
	}

	// Rebuild tree
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
	if (!debugDraw)
		return;

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
	CommonUtils::DrawBox(node->GetCentre(), node->GetSize().x, 0.05f);

	// Draw children (if they exist)
	for (int i = 0; i < NUM_OCTANTS; ++i) {
		if (node->children[i]) {
			DebugDraw(node->children[i]);
		}
	}
}





void Octtree::Update() {

	//UpdateNonStaticObjects(root); 
	
	//UpdateRecombine(root);
	//
	//UpdateDivision(root);
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
	// Can't recombine an undivided node 
	if (!node->hasChildren || node == root) { 
		return;
	}

	for (int i = 0; i < NUM_OCTANTS; ++i) { // Recombine from bottom up
		UpdateRecombine(node->children[i]);
	}

	// Don't want to recombine the root  (only its children) even if root contains few objects
	if (node != root && CountChildren(node) < maxOctantObjects) {
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
	pairList.clear();
	BuildPotentialCollisionList(root, pairList);
	return pairList;
}

void Octtree::BuildPotentialCollisionList(OcttreeNode* node, std::vector<CollisionPair*>& pairList) {
	if (node->hasChildren) { 
		for (int i = 0; i < NUM_OCTANTS; ++i) {
			BuildPotentialCollisionList(node->children[i], pairList);
		}
	}
	else {
		if (node->objects.size() > 1) {
			for (int firstObj = 0; firstObj < node->objects.size(); ++firstObj) {
				for (int secondObj = firstObj + 1; secondObj < node->objects.size(); ++secondObj) {
					AddCollisionPair(node->objects[firstObj], node->objects[secondObj]);
				}
			}
		}
	}

}

void Octtree::AddCollisionPair(PhysicsNode* a, PhysicsNode* b) {
	// Don't add two stationary pairs
	if (a->IsStationary() && b->IsStationary()) {
		return;
	}
	// Don't re-add already added pairs
	for (const auto& pair : pairList) {
		if (pair->pObjectA == a || pair->pObjectB == a) {
			if (pair->pObjectA == b || pair->pObjectB == b) {
				return;
			}
		}
	}
	pairList.push_back(new CollisionPair(a, b));
}

void Octtree::DeleteTree(OcttreeNode* node) {
	if (!node) {
		return;
	}

	if (node->hasChildren) {
		for (int i = 0; i < NUM_OCTANTS; ++i) {
			DeleteTree(node->children[i]);
		}
	}
	SAFE_DELETE(node);
}