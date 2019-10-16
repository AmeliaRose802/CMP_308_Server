/*	AG0907 Lab 4 UDP server example - by Henry Fortuna and Adam Sampson

	When the user types a message, the client sends it to the server
	as a UDP packet. The server then sends a packet back to the
	client, and the client prints it out.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "Message.h"
#include <iostream>
#include <vector>
#include <map>
#include "GameObject.h"
#include <iterator> 
#include <set>
#include "HelperFunctions.h"

#pragma comment(lib, "ws2_32.lib")



// The IP address for the server
#define SERVERIP "127.0.0.1"

// The UDP port number for the server
#define SERVERPORT 4444

// The (fixed) size of message that we send between the two programs
#define MESSAGESIZE 40

#define LOOP_DELAY 1000


// Prototypes
void die(const char *message);




int main()
{

	//Holds all gameobjects
	std::map<int, GameObject> gameObjects;

	//Holds clients who are connected
	std::set<sockaddr_in> connections;

	timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	fd_set readable;
	FD_ZERO(&readable);

	printf("Game Server\n");

	// Initialise the WinSock library -- we want version 2.2.
	WSADATA w;
	int error = WSAStartup(0x0202, &w);
	if (error != 0)
	{
		die("WSAStartup failed");
	}
	if (w.wVersion != 0x0202)
	{
		die("Wrong WinSock version");
	}

	// Create a UDP socket.
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
	{
		die("socket failed");
	}

	// Fill out a sockaddr_in structure to describe the address we'll listen on.
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(SERVERIP);
	// htons converts the port number to network byte order (big-endian).
	serverAddr.sin_port = htons(SERVERPORT);

	// Bind the socket to that address.
	if (bind(sock, (const sockaddr *) &serverAddr, sizeof(serverAddr)) != 0)
	{
		die("bind failed");
	}

	// ntohs does the opposite of htons.
	printf("Server socket bound to address %s, port %d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));

	
	while (true)
	{
		FD_SET(sock, &readable);

		int count = select(0, &readable, NULL, NULL, &timeout);

		if (FD_ISSET(sock, &readable))
		{
			sockaddr_in fromAddr;
			int fromAddrSize = sizeof(fromAddr);

			Message msg(0, 0, 0);

			int count = recvfrom(sock, (char *)&msg, sizeof(Message), 0, (sockaddr *)&fromAddr, &fromAddrSize);
			if (count < 0)
			{
				die("recvfrom failed");
			}
			if (count != sizeof(Message))
			{
				die("received odd-sized message");
			}

			
			if (!connections.count(fromAddr)) {
				connections.insert(fromAddr);
			}
			
			//If this gameobject is already in game update postion otherwise add it
			if (gameObjects.count(msg.objectID)) {
				gameObjects[msg.objectID].update(msg.x, msg.y);
			}
			else {
				std::cout << "New object joined game\n";
				gameObjects.insert(std::pair<int, GameObject>(msg.objectID, GameObject(msg.objectID, msg.x, msg.y)));
			}
			
			if (sendToAllClients(msg, connections, sock)) {
				die("Send Failed");
			}

			printMap(gameObjects);
		}
	}

	// We won't actually get here, but if we did then we'd want to clean up...
	printf("Quitting\n");
	closesocket(sock);
	WSACleanup();
	return 0;
}


// Print an error message and exit.
void die(const char *message)
{
	fprintf(stderr, "Error: %s (WSAGetLastError() = %d)\n", message, WSAGetLastError());

#ifdef _DEBUG
	// Debug build -- drop the program into the debugger.
	abort();
#else
	exit(1);
#endif
}