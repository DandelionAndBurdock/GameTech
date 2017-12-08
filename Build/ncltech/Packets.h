#pragma once

#include <limits>
#include <nclgl/Vector3.h>
#include <string>

namespace Packets {
	enum PacketType { TEST_PACKET, POS_DATA, GEN_MAZE, MAZE_SEED};

	// All packets must derive from Packet as we will cast message to determine what type
	// of packet we have
	struct Packet {
		Packet::PacketType  message;

		virtual uint GetSize() = 0; // Return size in bytes
	};

	struct PacketVec3 : public Packet {
		Vector3 vec;

		uint GetSize() override { return sizeof(this); }
	};

	struct PacketIntFloat : public Packet {
		int i;
		float f;

		uint GetSize() override { return sizeof(this); }
	};

	struct PacketString : public Packet {
		std::string str;

		uint GetSize() override { return sizeof(this); }
	};

	struct PacketUint : public Packet {
		uint i;

		uint GetSize() override { return sizeof(this); }
	};
};

