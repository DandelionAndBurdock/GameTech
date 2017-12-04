// Since I am using positional collision response need different constraint structure 
// from the framework
#pragma once
#include <nclgl\Vector3.h>

#include <vector>

class SoftBodyConstraint
{
public:
	SoftBodyConstraint();
	virtual ~SoftBodyConstraint();
};


struct PinConstraint : public SoftBodyConstraint {
	Vector3 m_position;	// Position to remain pinned to
	int m_index;		// Index of mesh to pin

	PinConstraint(int index, Vector3 position)
		: m_position(position), m_index(index) {}
};

struct PairConstraint : public SoftBodyConstraint{
	float m_restLength;		// Ideal distance to maintain between the two  points
	int m_indexFirst;		// Index of first point on mesh
	int m_indexSecond;		// Index of second point on the mesh
	PairConstraint(float restLength, int indexFirst, int indexSecond) 
		: m_restLength(restLength), m_indexFirst(indexFirst), m_indexSecond(indexSecond) {}
};

// Some default constraints to make meshes with different characteristics depending which indices
// we constrain 
std::vector<PairConstraint*> MakeSquareLattice(int latticeDimension, float restLength);
std::vector<PairConstraint*> MakeCubeLattice(int latticeDimension, float restLength);