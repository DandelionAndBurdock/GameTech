#pragma once

#include <nclgl\Mesh.h>
#include "SoftBodyConstants.h"
#include "SoftBodyConstraint.h"


class Cloth : public Mesh
{
public:
	Cloth(GLint dimension = DEFAULT_GRID_SIZE,
		GLfloat length = DEFAULT_LENGTH);
	~Cloth();

	void Draw() override;

	void Update(float dt);

	// Create a new constraint pinning index i at its current position
	void Pin(int index);
protected:
	// Grid properties
	GLuint m_dimension;
	GLfloat m_length;
	GLfloat m_tileSize;
	GLint m_numPoints;

	// Construct vertices, indices, texture coordinates etc for mesh
	void BuildMesh();

	// Send updated positions to graphics card
	void Rebuffer();

	// Physics
	// Add some artificial damping into the system
	float m_energyLossFactor = ENERGY_LOSS_FACTOR;
	// Contains acceleration at each point on the mesh calculated in accumulated forces function
	Vector3* m_acceleration;
	// Contains current position at each point on the mesh
	Vector3* m_currentPosition;
	// Contains position at each point on the mesh in the previous physics update
	Vector3* m_oldPosition;

	// Constraints
	std::vector<PinConstraint*> m_pinConstraints;
	std::vector<PairConstraint*> m_pairConstraints;

	// Sum gravity, wind, etc...
	void AccumulateForces(float dt);
	// Numerical integration of points in mesh
	void Integrate(float dt);
	// Use relaxation to converge towards a valid configuration
	void SatisfyConstraints();

	bool m_wireframe = true;

};

