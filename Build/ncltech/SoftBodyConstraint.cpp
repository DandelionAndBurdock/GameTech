#include "SoftBodyConstraint.h"



SoftBodyConstraint::SoftBodyConstraint()
{
}


SoftBodyConstraint::~SoftBodyConstraint()
{
}

// Pair up each index with its vertical and horizontal neighbour
std::vector<PairConstraint*> MakeSquareLattice(int latticeDimension, float restLength) {
	std::vector<PairConstraint*> temp;
	for (int i = 0; i < latticeDimension * latticeDimension; ++i){
		if ((i + 1) % latticeDimension) { // Not for last column
			temp.push_back(new PairConstraint(restLength, i, i + 1));
		}

		if (((i + latticeDimension) / latticeDimension) < latticeDimension) {// Not for last row
			temp.push_back(new PairConstraint(restLength, i, i + latticeDimension));
		}
	}
	return temp;
}


std::vector<PairConstraint*> MakeCubeLattice(int latticeDimension, float restLength) {
	std::vector<PairConstraint*> temp;
	for (int i = 0; i < 6 * latticeDimension * latticeDimension; ++i) {
		if ((i + 1) % latticeDimension) { // Not for last column
			temp.push_back(new PairConstraint(restLength, i, i + 1));
		}

		if (((i + latticeDimension) / latticeDimension) < i * latticeDimension) {// Not for last row
			temp.push_back(new PairConstraint(restLength, i, i + latticeDimension));
		}
	}
	return temp;
}