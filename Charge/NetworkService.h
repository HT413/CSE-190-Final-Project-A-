#ifndef _NETWORK_COMMS_SERVICE_
#define _NETWORK_COMMS_SERVICE_

#include <winsock2.h>
#include <Windows.h>

class NetworkService
{
public:
	static int sendMessage(SOCKET curSocket, char * message, int messageSize);
	static int receiveMessage(SOCKET curSocket, char * buffer, int bufSize);
};

#endif