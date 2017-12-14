#pragma once

#include <ncltech\AIObject.h>

#include <ncltech\MazeGenerator.h>
#include <list>



class Hazard : public AIObject
{
public:
	Hazard(std::string name);
	~Hazard();
	void Update(float dt) override;
	void SetPath(std::list<const GraphNode*> pathList);

	StateMachineManager<Hazard>* GetFSM() { return fsm; }
protected:
	// List of nodes on the path
	std::vector<const GraphNode*> path;
	// Current index in path
	int currentPathIndex;

	// Hazard States
	StateMachineManager<Hazard>* fsm;
};

