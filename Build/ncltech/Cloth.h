#pragma once

#include <nclgl\Mesh.h>
#include "SoftBodyConstants.h"


class Cloth : public Mesh
{
public:
	Cloth(GLint dimension = DEFAULT_GRID_SIZE,
		GLfloat length = DEFAULT_LENGTH);
	~Cloth();

	void Draw() override;

	void Update(float dt);

protected:
	// Grid properties
	GLuint m_dimension;
	GLfloat m_length;
	GLfloat m_tileSize;
	GLint m_numPoints;

	// Construct vertices, indices, texture coordinates etc for mesh
	void BuildMesh();

	// Physics
	// Contains acceleration at each point on the mesh calculated in accumulated forces function
	Vector3* m_acceleration;
	// Contains current position at each point on the mesh
	Vector3* m_currentPosition;
	// Contains position at each point on the mesh in the previous physics update
	Vector3* m_oldPosition;

	// Sum gravity, wind, etc...
	void AccumulateForces(float dt);
	// Numerical integration of points in mesh
	void Integrate(float dt);

	//void SatisfyConstraints();

	bool m_wireframe = true;

};

