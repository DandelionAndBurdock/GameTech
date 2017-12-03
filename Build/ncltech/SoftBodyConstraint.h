// Since I am using positional collision response need different constraint structure 
// from the framework
#pragma once
#include <nclgl\Vector3.h>

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