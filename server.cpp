/*	AG0907 Lab 2 TCP server example - by Henry Fortuna and Adam Sampson

	A simple TCP server that waits for a connection.
	When a connection is made, the server sends "hello" to the client.
	It then repeats back anything it receives from the client.
	All the calls are blocking -- so this program only handles
	one connection at a time.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <iostream>
#include <sstream>
#include <iomanip>

#pragma comment(lib, "ws2_32.lib")


// The IP address for the server to listen on
#define SERVERIP "127.0.0.1"

// The TCP port number for the server to listen on
#define SERVERPORT 5555

// The message the server will send when the client connects
#define WELCOME "hello"

// The (fixed) size of message that we send between the two programs
#define MESSAGESIZE 40

#define ERROR_VALUE -1

// Prototypes
void die(const char *message);


int main()
{
	printf("Echo Server\n");

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

	// Create a TCP socket that we'll use to listen for connections.
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	// Test for error creating socket
	if (serverSocket == INVALID_SOCKET) {
		die("Creating Server Socket Failed: " + WSAGetLastError());
	}

	// Fill out a sockaddr_in structure to describe the address we'll listen on.
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(SERVERIP);
	// htons converts the port number to network byte order (big-endian).
	serverAddr.sin_port = htons(SERVERPORT);

	// Bind the server socket to that address.

	
	if (bind(serverSocket, (const sockaddr *) &serverAddr, sizeof(serverAddr)) != 0)
	{
		die("bind failed");
	}

	// ntohs does the opposite of htons.
	printf("Server socket bound to address %s, port %d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));

	
	// Make the socket listen for connections.
	if (listen(serverSocket, 1) != 0)
	{
		die("listen failed");
	}

	printf("Server socket listening\n");

	while (true)
	{
		printf("Waiting for a connection...\n");

		// Accept a new connection to the server socket.
		// This gives us back a new socket connected to the client, and
		// also fills in an address structure with the client's address.
		sockaddr_in clientAddr;
		int addrSize = sizeof(clientAddr);
		SOCKET clientSocket = accept(serverSocket, (sockaddr *) &clientAddr, &addrSize);


		//If the cleint socket is invalid print an error message and return to listening
		if (clientSocket == INVALID_SOCKET)
		{
			printf("***Invalid client socket***\n");
		
		}
		else {
			printf("Client has connected from IP address %s, port %d!\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

			// We'll use this array to hold the messages we exchange with the client.
			char buffer[MESSAGESIZE];
			char * messageBuffer  = NULL;

			// Fill the buffer with - characters to start with.
			memset(buffer, '-', MESSAGESIZE);

			// Send a welcome message to the client.
			memcpy(buffer, WELCOME, strlen(WELCOME));
			
			// Check for error from send
			if (send(clientSocket, buffer, MESSAGESIZE, 0) == ERROR_VALUE) {
				die("Send failed: " + WSAGetLastError());
			}

			while (true)
			{
				char  messageLength[5];
				recv(clientSocket, messageLength, 5, 0);

				

				int length;
				std::istringstream(messageLength) >> length;

				std::cout << "Message Length: " << length;
				
				messageBuffer = new char[length];
				// Receive as much data from the client as will fit in the buffer.
				int count = recv(clientSocket, messageBuffer, length, 0);

				std::stringstream ss;
				ss << std::setw(5) << std::setfill('0') << length;
				std::string lineLength = ss.str();

				std::string line = messageBuffer;

				line.insert(0, lineLength);


				std::cout <<"Message Buffer: "<< line;
				
				// Check for errors from recv
				if (count == ERROR_VALUE) {
					die("recv failed: " + WSAGetLastError());
				}

				if (count <= 0) {
					printf("Client closed connection\n");
					break;
				}
				if (count != length) {
					die("Got strange-sized message from client");
				}
				if (memcmp(buffer, "quit", 4) == 0) {
					printf("Client asked to quit\n");
					break;
				}

				// (Note that recv will not write a \0 at the end of the message it's
				// received -- so we can't just use it as a C-style string directly
				// without writing the \0 ourself.)

				printf("Received %d bytes from the client: '", count);
				fwrite(messageBuffer, 1, count, stdout);
				printf("'\n");

				char * replyBuffer = new char[line.length()];
				
				// Send the same data back to the client.
				memcpy(replyBuffer, line.c_str(), line.length());

				// Check for error from send
				if (send(clientSocket, replyBuffer, line.length(), 0) == ERROR_VALUE) {
					die("Send failed: " + WSAGetLastError());
				}
				
			}

			printf("Closing connection\n");

			// Close the connection.
			closesocket(clientSocket);
		}

	}

	// We won't actually get here, but if we did then we'd want to clean up...
	printf("Quitting\n");
	closesocket(serverSocket);
	WSACleanup();
	return 0;
}


// Print an error message and exit.
void die(const char *message) {
	fprintf(stderr, "Error: %s (WSAGetLastError() = %d)\n", message, WSAGetLastError());

#ifdef _DEBUG
	// Debug build -- drop the program into the debugger.
	abort();
#else
	exit(1);
#endif
}