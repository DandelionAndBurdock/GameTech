#pragma once

#include <vector>

#include "PhysicsNode.h"
#include "BoundingCube.h"


class Octtree
{
	// In case I forget Oct means 8
	static const int NUM_OCTANTS = 8;

	struct OcttreeNode {
		// Region of this node
		BoundingCube* region;
		// Lists of objects in this node
		std::vector<PhysicsNode*> objects;
		// Parent node
		OcttreeNode* parent;
		// Child Nodes 
		OcttreeNode* children[NUM_OCTANTS];
		// Has children
		bool hasChildren = false;
		
		OcttreeNode(Vector3 minCorner, Vector3 maxCorner) {
			region = new BoundingCube(minCorner, maxCorner);
			for (int i = 0; i < NUM_OCTANTS; ++i) {
				children[i] = nullptr;
			}
			parent = nullptr;
		}
		void AddChild(OcttreeNode* child, int childIndex) {
			children[childIndex] = child;
			child->parent = this;
			hasChildren = true;
		}

		// Getters
		Vector3 GetCentre() const { return region->GetCentre(); }
		Vector3 GetSize() const { return region->GetSize(); }
		Vector3 GetMinCorner() const { return region->GetMinCorner(); }
	};
public:
	Octtree(Vector3 minCorner, Vector3 maxCorner);
	~Octtree();

	void InsertObjects(std::vector<PhysicsNode*> nodes);
	void InsertObject(PhysicsNode* node);
	void BuildTree();

	void SetMinCubeSize(float s) { minCubeSize = s; }
	void SetMaxOctantObjects(int n) { maxOctantObjects = n; }

	void Update();

	void DebugDraw();

	bool AtMinimumSize(OcttreeNode* node);
protected:
	// Lists of all objects in the octtree
	std::vector<PhysicsNode*> objects;

	// Root of the tree
	OcttreeNode* root;

	// World dimensions
	Vector3 worldMin;
	Vector3 worldMax;

	// Recusion will halt if an octant has a dimension lower minCubeSize
	float minCubeSize = 1.0f;
	// Threshold for the max number of objects in one octant (assuming size greater than minCubeSize)
	int maxOctantObjects = 4;

protected:
	// Divide octant into 8 further child octants
	void Divide(OcttreeNode* node);

	// Allocate all objects in this node into the correct child node
	void SendObjectsToChildren(OcttreeNode* node);

	// Insert an object recursively into the correct 
	void InsertObject(PhysicsNode* object, OcttreeNode* node);
	// Inserts an object recursively into the tree but without triggering any change in the tree structure
	void SimpleInsertObject(PhysicsNode* object, OcttreeNode* node);
	// // Move objects into new octants (without division)
	void UpdateNonStaticObjects(OcttreeNode* node);
	// Divide any undivided octants which now exceed threshold					  
	void UpdateDivision(OcttreeNode* node);
	// Recombine any cells which no longer exceed threshold
	void UpdateRecombine(OcttreeNode* node);
	// Sums up all objects contained by the children of this node
	int CountChildren(OcttreeNode* node);
protected:
	// Recursive draw function
	void DebugDraw(OcttreeNode* node);
	//Constructs the static cube hull 
	static void ConstructCubeHull();
	// For debug drawing
	static Hull	 cubeHull;		

};

