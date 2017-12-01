#include "BoundingCube.h"



BoundingCube::BoundingCube(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax)
{
	this->xMin = xMin;
	this->yMin = yMin;
	this->zMin = zMin;
	this->xMax = xMax;
	this->yMax = yMax;
	this->zMax = zMax;

	minCorner = Vector3(xMin, yMin, zMin);
	maxCorner = Vector3(xMax, yMax, zMax);

	centre = minCorner + (maxCorner - minCorner) / 2.0f;
}

BoundingCube::BoundingCube(Vector3 minCorner, Vector3 maxCorner) {
	xMin = minCorner.x;
	yMin = minCorner.y;
	zMin = minCorner.z;

	xMax = maxCorner.x;
	yMax = maxCorner.y;
	zMax = maxCorner.z;

	this->minCorner = minCorner;
	this->maxCorner = maxCorner;

	centre = minCorner + (maxCorner - minCorner) / 2.0f;
}

BoundingCube::~BoundingCube()
{
}


// Returns true if sphere is inside the cube and false otherwise
bool BoundingCube::IsSphereInCube(Vector3 centre, float radius) {
	// Minimum/maximum possible sphere extents 
	Vector3 min = centre - Vector3(radius);
	Vector3 max = centre + Vector3(radius);
	// If sphere fails test on any axis it cannot be inside the box
	// If it doesn't fail any test it must be inside the box
	if (min.x > xMax || max.x < xMin) {
		return false;
	}
	if (min.y > yMax || max.y < yMin) {
		return false;
	}
	if (min.z > zMax || max.z < zMin) {
		return false;
	}
	return true;
}