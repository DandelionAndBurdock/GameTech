#pragma once

#include <limits>
#include <nclgl/Vector3.h>
#include <string>

namespace Packets {
	enum PacketType { TEST_PACKET, POS_DATA, GEN_MAZE, MAZE_SEED};

	// All packets must derive from Packet as we will cast message to determine what type
	// of packet we have
	struct Packet {
		PacketType  message;

		Packet(PacketType m) : message(m) {}
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
	};

	struct PacketString : public Packet {
		std::string str;

		PacketString(PacketType m, std::string s) :
			Packet(m), str(s) {}
	};
};

