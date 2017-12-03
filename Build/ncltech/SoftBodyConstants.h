#pragma once

#include <nclgl/Vector3.h>

const unsigned int DEFAULT_GRID_SIZE = 32;
const float DEFAULT_LENGTH = 6.0f;	// Length of one row/column
const float DEFAULT_TILE_SIZE = (float)DEFAULT_LENGTH / (DEFAULT_GRID_SIZE - 1);
const int NUM_ITERATIONS = 8; // Number of relaxations loops
const Vector3 GRAVITY = Vector3(0.0f, -0.5f, 0.0f);
const float ENERGY_LOSS_FACTOR = 0.99f;
