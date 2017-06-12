#ifndef _NETWORK_COMM_GAME_CLIENT_
#define _NETWORK_COMM_GAME_CLIENT_

#include <winsock2.h>
#include <Windows.h>
#include "ClientNetwork.h"
#include "NetworkData.h"

extern enum ACTOR_TYPE;

class ClientGame
{
public:
	ClientGame();
	~ClientGame();

	ClientNetwork* network;

	void sendActionPackets();
	void sendHandPos(float, float, float);
	void sendUnitCreation(float, float);
	void sendUnitPickup(float);
	void sendUnitPlaced();

	char network_data[MAX_PACKET_SIZE];

	void update();
};

#endif