#include "ClientGame.h"
#include "Globals.h"
#include "Window.h"
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
	float theX = -x;
	float theZ = -z;
	theZ -= 5.f;
	ss << "ZZ" << "," << theX << "," << y << "," << theZ;
	const char * str = ss.str().c_str();
	char packet_data[packet_size];
	strcpy(packet_data + 4, str);

	Packet packet;
	packet.packet_type = LEAP_HAND_LOC;
	
	packet.serialize(packet_data);
	NetworkService::sendMessage(network->ConnectSocket, packet_data, packet_size);
}

void ClientGame::sendUnitCreation(float type, float id){
	const unsigned int packet_size = 33 * sizeof(Packet);
	std::ostringstream ss;
	ss << "ZZ" << "," << type << "," << id ;

	char* cstr = new char[32 * sizeof(Packet)];
	std::strcpy(cstr, ss.str().c_str());

	char packet_data[packet_size];
	strcpy(packet_data + 4, cstr);

	Packet packet;
	packet.packet_type = LEAP_UNIT_CREATION;
	
	packet.serialize(packet_data);
	NetworkService::sendMessage(network->ConnectSocket, packet_data, packet_size);
	
	delete cstr;
}

void ClientGame::sendUnitPickup(float id) {
	const unsigned int packet_size = 33 * sizeof(Packet);
	std::ostringstream ss;
	ss << "ZZ" << "," << id;

	char* cstr = new char[32 * sizeof(Packet)];
	std::strcpy(cstr, ss.str().c_str());

	char packet_data[packet_size];
	strcpy(packet_data + 4, cstr);

	Packet packet;
	packet.packet_type = LEAP_UNIT_PICK_UP;

	packet.serialize(packet_data);
	NetworkService::sendMessage(network->ConnectSocket, packet_data, packet_size);
}

void ClientGame::sendUnitPlaced()
{
	// send action packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = LEAP_UNIT_PLACED_DOWN;

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
		case LEAP_UNIT_PLACED_DOWN:
		{
			// Ignore this data
			break;
		}

		case LEAP_UNIT_PICK_UP:
		{
			i += 32 * sizeof(Packet);
			// Ignore this data
			break;
		}

		case RIFT_UNIT_PLACED_DOWN:
		{
			unitPlacedown();
			break;
		}

		case RIFT_UNIT_PICK_UP:
		{
			std::stringstream ss;
			std::vector<std::string> unitValues;
			std::string split;

			char unitInfo[32 * sizeof(Packet)];
			for(int j = 0; j < 32 * sizeof(Packet); j++) {
				packet.deserialize(&(network_data[i + j]));
			}

			memcpy(unitInfo, network_data + i, 32 * sizeof(Packet));

			ss.str(unitInfo);
			while(std::getline(ss, split, ',')) {
				if(split.length() > 1) {	//ignore the first nonsense characters
					unitValues.push_back(split);
				}
			}

			if(unitValues.size() > 1) {
				int unitID = int(stof(unitValues[1]));
				cout << "Unit picked up has ID " << unitID << endl;
				unitPickup(unitID);
			}

			i += 32 * sizeof(Packet);

			break;
		}
		case RIFT_UNIT_CREATION:
		{
			std::stringstream ss;
			std::vector<std::string> unitValues;
			std::string split;

			char unitInfo[32 * sizeof(Packet)];
			for(int j = 0; j < 32 * sizeof(Packet); j++) {
				packet.deserialize(&(network_data[i + j]));
			}

			memcpy(unitInfo, network_data + i, 32 * sizeof(Packet));

			ss.str(unitInfo);
			while(std::getline(ss, split, ',')) {
				//split contains the coorindates of hand position
				if(split.length() > 1) {	//ignore the first nonsense characters
					unitValues.push_back(split);
				}
			}

			if(unitValues.size() > 1) {
				ACTOR_TYPE unitType = static_cast<ACTOR_TYPE>(int(stof(unitValues[1])));
				int unitID = int(stof(unitValues[2]));
				createNewUnit(unitType, unitID);
			}
			
			i += 32 * sizeof(Packet);
			break;
		}

		case RIFT_HAND_LOC:
		{
			/* data structures to split string by ',' */
			std::stringstream ss;
			std::vector<std::string> handPosValues;
			std::string split;


			char loc[32 * sizeof(Packet)];
			for(int j = 0; j < 32 * sizeof(Packet); j++) {
				packet.deserialize(&(network_data[i + j]));
			}
			memcpy(loc, network_data + i, 32 * sizeof(Packet));

			ss.str(loc);
			while(std::getline(ss, split, ',')) {
				//split contains the coorindates of hand position
				if(split.length() > 2) {	//ignore the first nonsense characters
					handPosValues.push_back(split);
				}
			}

			vec3 theHandPosition;
			if(handPosValues.size() > 2) {
				theHandPosition.x = std::stof(handPosValues[0]);	//stof converts string to float
				theHandPosition.y = std::stof(handPosValues[1]);
				theHandPosition.z = std::stof(handPosValues[2]);
			}

			i += 32 * sizeof(Packet);
			setRiftHand(vec3(theHandPosition.x, theHandPosition.y, theHandPosition.z));
			break;
		}

		case RIFT_HAND_ORI:
		{
			/* data structures to split string by ',' */
			std::stringstream ss;
			std::vector<std::string> handPosValues;
			std::string split;


			char loc[32 * sizeof(Packet)];
			for(int j = 0; j < 32 * sizeof(Packet); j++) {
				packet.deserialize(&(network_data[i + j]));
			}
			memcpy(loc, network_data + i, 32 * sizeof(Packet));

			ss.str(loc);
			while(std::getline(ss, split, ',')) {
				//split contains the coorindates of hand position
				if(split.length() > 2) {	//ignore the first nonsense characters
					handPosValues.push_back(split);
				}
			}

			vec3 theHandPosition;
			if(handPosValues.size() > 2) {
				theHandPosition.x = std::stof(handPosValues[0]);	//stof converts string to float
				theHandPosition.y = std::stof(handPosValues[1]);
				theHandPosition.z = std::stof(handPosValues[2]);
			}

			i += 32 * sizeof(Packet);
			//setRiftHandOri(vec3(theHandPosition.x, theHandPosition.y, theHandPosition.z));
			break;
		}

		case RIFT_HEAD_LOC:
		{
			/* data structures to split string by ',' */
			std::stringstream ss;
			std::vector<std::string> headPosValues;
			std::string split;


			char loc[32 * sizeof(Packet)];
			for(int j = 0; j < 32 * sizeof(Packet); j++) {
				packet.deserialize(&(network_data[i + j]));
			}
			memcpy(loc, network_data + i, 32 * sizeof(Packet));

			ss.str(loc);
			while(std::getline(ss, split, ',')) {
				//split contains the coorindates of hand position
				if(split.length() > 2) {	//ignore the first nonsense characters
					headPosValues.push_back(split);
				}
			}

			vec3 theHeadPosition;
			if(headPosValues.size() > 2) {
				theHeadPosition.x = std::stof(headPosValues[0]);	//stof converts string to float
				theHeadPosition.y = std::stof(headPosValues[1]);
				theHeadPosition.z = std::stof(headPosValues[2]);
			}

			i += 32 * sizeof(Packet);
			setRiftHead(vec3(theHeadPosition.x, theHeadPosition.y, theHeadPosition.z));
			break;
		}

		case RIFT_HEAD_ORI:
		{
			/* data structures to split string by ',' */
			std::stringstream ss;
			std::vector<std::string> headPosValues;
			std::string split;


			char loc[32 * sizeof(Packet)];
			for(int j = 0; j < 32 * sizeof(Packet); j++) {
				packet.deserialize(&(network_data[i + j]));
			}
			memcpy(loc, network_data + i, 32 * sizeof(Packet));

			ss.str(loc);
			while(std::getline(ss, split, ',')) {
				//split contains the coorindates of hand position
				if(split.length() > 2) {	//ignore the first nonsense characters
					headPosValues.push_back(split);
				}
			}

			vec3 theHeadPosition;
			if(headPosValues.size() > 2) {
				theHeadPosition.x = std::stof(headPosValues[0]);	//stof converts string to float
				theHeadPosition.y = std::stof(headPosValues[1]);
				theHeadPosition.z = std::stof(headPosValues[2]);
			}

			i += 32 * sizeof(Packet);
			//setRiftHeadOri(vec3(theHeadPosition.x, theHeadPosition.y, theHeadPosition.z));
			break;
		}

		case LEAP_HAND_ORI:
		{
			i += 32 * sizeof(Packet);
			// Ignore this
			break;
		}

		case LEAP_UNIT_CREATION:
		{
			i += 32 * sizeof(Packet);
			// Ignore this
			break;
		}

		case LEAP_HAND_LOC:
		{
			i += 32 * sizeof(Packet);
			// Ignore this
			break;
		}

		case GAME_START_NOTICE:
			gameStart = true;
			printf("Opponent found! Game now starting!\n");
			break;

		case ACTION_EVENT:
			//printf("Server ping was successful!\n");
			//sendActionPackets();
			break;

		case INIT_CONNECTION:
			break;

		default:
			//printf("Error in packet types\n");
			break;
		}
	}
}