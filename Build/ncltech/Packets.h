#pragma once

#include <limits>
#include <nclgl/Vector3.h>
#include <string>
#include <sstream>

#include "MazeGenerator.h"

namespace Packets {
	enum PacketType { TEST_PACKET,
					  POS_DATA, 
					  MAZE_PARAM,	  // Contains dimensions and density of the maze
					  MAZE_STRUCTURE, // Contains nodes and edges of the maze
					  REGEN_MAZE,	  // Create a new maze with current parameter
					  ROUTE_REQUEST,  // Contains start and end node
					  ROUTE_NODES,	  // Contains a list of root nodes
	};	

	// All packets must derive from Packet as we will cast message to determine what type
	// of packet we have
	struct Packet {
		PacketType  message;

		Packet(PacketType m) : message(m) {}
		Packet() {}
	};

	struct PacketVec3 : public Packet {
		Vector3 vec;

		PacketVec3(PacketType m, Vector3 v) : 
			Packet(m), vec(v) {}
	};

	struct PacketIntFloat : public Packet {
		int i;
		float f;

		PacketIntFloat(PacketType m, int i, float f) :
			Packet(m), i(i), f(f) {}
		PacketIntFloat() {}
	};

	struct PacketString : public Packet {
		std::string str;

		PacketString(PacketType m, std::string s) :
			Packet(m), str(s) {}
	};

	struct PacketInt : public Packet {
		int i;

	PacketInt(PacketType m, int i) :
			Packet(m), i(i) {}
	};

	struct PacketNode : public Packet {
		GraphNode* node;

		PacketNode(PacketType m, GraphNode* n) :
			Packet(m), node(n) {}
	};

};

