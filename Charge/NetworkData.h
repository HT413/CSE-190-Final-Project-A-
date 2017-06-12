#ifndef _NETWORK_COMM_DATA_
#define _NETWORK_COMM_DATA_

#include <string.h>

#define MAX_PACKET_SIZE 1000000

enum PacketTypes {
	INIT_CONNECTION = 0,
	ACTION_EVENT = 1,
	GAME_START_NOTICE = 2,
	RIFT_HAND_LOC = 3,
	RIFT_HEAD_LOC = 4,
	LEAP_HAND_LOC = 5,
	UNIT_CREATION = 6
};

struct Packet {

	unsigned int packet_type;

	void serialize(char * data) {
		memcpy(data, this, sizeof(Packet));
	}

	void deserialize(char * data) {
		memcpy(this, data, sizeof(Packet));
	}
};

#endif