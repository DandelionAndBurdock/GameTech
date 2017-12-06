#pragma once

#include <limits>
#include <nclgl/Vector3.h>
#include <string>

namespace Packet {
	enum PacketType { TEST_PACKET, POS_DATA, GEN_MAZE,};

	// All packets must derive from Packet as we will cast message to determine what type
	// of packet we have
	struct Packet {
		Packet::PacketType  message;
	};

	struct PacketVec3 : public Packet {
		Vector3 vec;
	};

	struct PacketIntFloat : public Packet {
		int i;
		float f;
	};

	struct PacketString : public Packet {
		std::string str;
	};
};

