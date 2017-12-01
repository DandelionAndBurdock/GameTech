#pragma once

#include "nclgl/Vector3.h"
class BoundingCube
{
public:
	BoundingCube(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax);
	BoundingCube(Vector3 minCorner, Vector3 maxCorner);
	~BoundingCube();

	// Returns true if sphere is inside the cube and false otherwise
	bool IsSphereInCube(Vector3 centre, float radius);

	Vector3 GetCentre() const { return centre; }
	Vector3 GetSize() const { return maxCorner - minCorner; }
	Vector3 GetMinCorner() const { return minCorner; }
protected:
	float xMin;
	float xMax;
	float yMin;
	float yMax;
	float zMin;
	float zMax;

	Vector3 minCorner;
	Vector3 maxCorner;
	Vector3 centre;
};

