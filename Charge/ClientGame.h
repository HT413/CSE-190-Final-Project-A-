#ifndef _NETWORK_COMM_GAME_CLIENT_
#define _NETWORK_COMM_GAME_CLIENT_

#include <winsock2.h>
#include <Windows.h>
#include "ClientNetwork.h"
#include "NetworkData.h"

class ClientGame
{
public:
	ClientGame();
	~ClientGame();

	ClientNetwork* network;

	void sendActionPackets();

	char network_data[MAX_PACKET_SIZE];

	void update();
};

#endif