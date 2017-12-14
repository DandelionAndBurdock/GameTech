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
					  MAZE_ROUTE,	  // Contains a list of indices to route nodes
					  CREATE_AVATAR,  // Contains a node index to create an avatar
					  AVATAR_POS,	  // Position of avatar in maze space
					  ADD_HAZARD,	  // Contains node index to add a hazard
					  ADD_HAZARD_REQUEST, // Requests the server to add a hazard
					  HAZARD_POS,	 // Contains ID of hazard and position
					  ADD_SECONDARY_AVATAR, // Request to add a non-player avatar to the client
					  SEC_AVATAR_UPDATE,	// Contains ID and new position of a secondary avatar
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

	struct PacketCharArray : public Packet {
		char data[1024];

		PacketCharArray(PacketType m) :
			Packet(m)  {}
	};

	struct PacketIntVec3 : public Packet {
		int i;
		Vector3 vec;
		PacketIntVec3(PacketType m, int i, Vector3 v) :
			Packet(m), i(i), vec(v) {}
	};

	struct PacketNode : public Packet {
		GraphNode* node;

		PacketNode(PacketType m, GraphNode* n) :
			Packet(m), node(n) {}
	};

};

