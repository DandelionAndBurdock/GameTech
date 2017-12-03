#pragma once

#include <nclgl\Mesh.h>
#include "SoftBodyConstants.h"
#include "SoftBodyConstraint.h"

#include "SphereCollisionShape.h"

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

	// Add a sphere to possible collider list
	void AddSphere(SphereCollisionShape* s) { m_spheres.push_back(s); }
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

	// Collision Response
	// I will only do spheres but most shapes could be resolved in a similar (though more expensive) way
	std::vector<SphereCollisionShape*> m_spheres; // List of spheres in the scene
	void ResolveCollision(Vector3& clothPoint, SphereCollisionShape* sphere);

	// Sum gravity, wind, etc...
	void AccumulateForces(float dt);
	// Numerical integration of points in mesh
	void Integrate(float dt);
	// Use relaxation to converge towards a valid configuration
	void SatisfyConstraints();
	// Resolve any collisions (Special type of constraint)
	void ResolveCollisions();

	bool m_wireframe = true;

};

