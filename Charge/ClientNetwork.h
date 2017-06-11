#ifndef _NETWORK_COMM_CLIENT_
#define _NETWORK_COMM_CLIENT_

// Networking libraries
#include <winsock2.h>
#include <Windows.h>
#include "NetworkService.h"
#include <ws2tcpip.h>
#include <stdio.h> 
#include "NetworkData.h"

// size of our buffer
#define DEFAULT_BUFLEN 512
// port to connect sockets through 
#define DEFAULT_PORT "6881"
// IP address of server
#define SERVER_IP "128.54.201.244"
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class ClientNetwork
{

public:

	// for error checking function calls in Winsock library
	int iResult;

	// socket for client to connect to server
	SOCKET ConnectSocket;

	// ctor/dtor
	ClientNetwork();
	~ClientNetwork(){};

	int receivePackets(char *);
};

#endif