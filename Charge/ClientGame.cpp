#include "ClientGame.h"
#include "Globals.h"
#include <sstream>
#include <string>

ClientGame::ClientGame()
{
	network = new ClientNetwork();
	// Send init packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = INIT_CONNECTION;

	packet.serialize(packet_data);
	NetworkService::sendMessage(network->ConnectSocket, packet_data, packet_size);
}


ClientGame::~ClientGame()
{
}

void ClientGame::sendActionPackets()
{
	// send action packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = ACTION_EVENT;

	packet.serialize(packet_data);
	NetworkService::sendMessage(network->ConnectSocket, packet_data, packet_size);
}

void ClientGame::sendHandPos(float x, float y, float z){
	const unsigned int packet_size = 33 * sizeof(Packet);
	std::ostringstream ss;
	ss << "ZZ" << "," << x << "," << y << "," << z;
	const char * str = ss.str().c_str();
	char packet_data[packet_size];
	strcpy(packet_data + 4, str);

	Packet packet;
	packet.packet_type = LEAP_HAND_LOC;

	packet.serialize(packet_data);
	NetworkService::sendMessage(network->ConnectSocket, packet_data, packet_size);
}

void ClientGame::sendUnitCreation(ACTOR_TYPE type, int id){
	const unsigned int packet_size = 17 * sizeof(Packet);
	std::ostringstream ss;
	ss << "ZZ" << "," << type << "," << id;
	const char * str = ss.str().c_str();
	char packet_data[packet_size];
	strcpy(packet_data + 4, str);

	Packet packet;
	packet.packet_type = UNIT_CREATION;

	packet.serialize(packet_data);
	NetworkService::sendMessage(network->ConnectSocket, packet_data, packet_size);
}

void ClientGame::update()
{
	Packet packet;
	int data_length = network->receivePackets(network_data);

	if(data_length <= 0)
	{
		//no data recieved
		return;
	}

	int i = 0;
	while(i < (unsigned int)data_length)
	{
		packet.deserialize(&(network_data[i]));
		i += sizeof(Packet);

		switch(packet.packet_type) {
		case GAME_START_NOTICE:
			gameStart = !gameStart;
			printf("Opponent found! Game now starting!\n");
			break;

		case ACTION_EVENT:
			printf("Server ping was successful!\n");
			sendActionPackets();
			break;

		default:
			printf("Error in packet types\n");
			break;
		}
	}
}