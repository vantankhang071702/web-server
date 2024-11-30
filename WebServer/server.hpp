#ifndef SERVER_HPP
#define SERVER_HPP

#include <winsock2.h>        // For socket programming in Windows
#include <ws2tcpip.h>        // For inet_pton, etc.
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <cstring>           // For memset
#include <cstdlib>           // For exit()

#pragma comment(lib, "ws2_32.lib")  // Required to link the Winsock library

// Foward declaration
class ConnectionHandler;

class Server {
public:
	Server(int port = 8080); // By default, the port would be 8080
	~Server(); // Destructor

	void start(); // start the server
	void stop(); // stop the server

private:
	int port;				// Which port number the server is listen to
	int server_fd;			// Socket file descriptor for the server
	sockaddr_in server_addr;	// Address structure for the server
	bool running;			// Server status flag

	std::vector<std::thread> worker_threads; // Thread pool for handling client requests
	void setupServer(); // Initializes the server (bind, listen, etc.)
	void acceptConnection(); // Accepts new client connections
	void handleClient(SOCKET client_fd); // Handles individual client requests
	void closeServer(); // Closes the server ( closes sockets, etc. ) 
};


#endif // SERVER_HPP