#include "ClientGame.h"
#include "Globals.h"


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