#pragma once
#include <ncltech\GameObject.h>
#include <ncltech\Scene.h>
#include "SearchAlgorithm.h"

#include <ncltech\Serializable.h>

class MazeGenerator : public Serializable
{
public:
	MazeGenerator(); //Maze_density goes from 1 (single path to exit) to 0 (no walls at all)
	virtual ~MazeGenerator();

	void Generate(int size, float maze_density);

	//All points on the maze grid are connected in some shape or form
	// so any two nodes picked randomly /will/ have a path between them
	GraphNode* GetStartNode() const		{ return start; }
	GraphNode* GetGoalNode()  const		{ return end; }
	uint GetSize() const { return size; }


	//Used as a hack for the MazeRenderer to generate the walls more effeciently
	GraphNode* GetAllNodesArr() { return allNodes; }

	void Generate(int seed, int grid_size, float maze_density);

	void Serialize(std::ostream& stream);
	void Deserialize(std::istream& stream);

	// Returns index in allnodes or -1 if not present
	int GetIndexFromNode(GraphNode* node);
	GraphNode* GetNodeFromIndex(int index);
protected:
	void GetRandomStartEndNodes();

	void Initiate_Arrays();

	void Generate_Prims();
	void Generate_Sparse(float density);

	

public:
	uint size;

	uint startIndex;
	uint endIndex;
	GraphNode *start, *end;

	GraphNode* allNodes;
	GraphEdge* allEdges;
};