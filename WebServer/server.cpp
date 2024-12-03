#include "server.hpp"

// Link with ws2_32.lib
#pragma comment(lib, "ws2_32.lib") // Required to link the Winsock library

Server::Server(int port) : port(port), server_fd(INVALID_SOCKET), running(false) {
	memset(&server_addr, 0, sizeof(server_addr));
}

Server::~Server() {
	stop();
}

void Server::start() {
	setupServer();
	running = true;
	acceptConnection();
}

void Server::stop() {
	running = false;
	closeServer();

	for (auto& t : worker_threads) {
		if (t.joinable()) {
			t.join();
		}
	}
	std::cout << "Server stopped." << std::endl;
}

void Server::setupServer() {
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData); // Initialize Winsock with version 2.2

	if (result != 0) {
		std::cerr << "WSAStartup failed with error: " << result << std::endl;
		exit(1);
	}

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == INVALID_SOCKET) {
		std::cerr << "Socket creation failed!" << std::endl;
		WSACleanup();
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	// Bind the socket to the address and port
	if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		std::cerr << "Bind failed!" << std::endl;
		closesocket(server_fd);
		WSACleanup();
		exit(1);
	}

	if (listen(server_fd, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "Listent failed!" << std::endl;
		closesocket(server_fd);
		WSACleanup();
		exit(1);
	}

	std::cout << "Server listening on port " << port << std::endl;

	router.addRoute("GET", "/demo", std::bind(&Server::handleDemo, this, std::placeholders::_1));
}

void Server::acceptConnection() {
	SOCKET client_fd;
	sockaddr_in client_addr;
	int client_len = sizeof(client_addr);

	while (running) {
		client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

		if (client_fd == INVALID_SOCKET) {
			std::cerr << "Client accept failed!" << std::endl;
			continue;
		}

		std::cout << "New connection accepted!" << std::endl;

		// Handle the client connection in separate thread
		worker_threads.push_back(std::thread(&Server::handleClient, this, client_fd));
	}
}

void Server::handleClient(SOCKET client_fd) {
	char buffer[1024] = { 0 };
	int valread = recv(client_fd, buffer, sizeof(buffer), 0);

	if (valread > 0) {
		std::string request(buffer);

		size_t methodEnd = request.find(' ');
		std::string method = request.substr(0, methodEnd);
		size_t pathStart = request.find(' ') + 1;
		size_t pathEnd = request.find(' ', pathStart);
		std::string path = request.substr(pathStart, pathEnd - pathStart);

		if (!router.handleRequest(method, path, client_fd)) {
			sendErrorResponse(client_fd, 404);
		}
	}

	// Close the client socket after handling
	closesocket(client_fd);
}

void Server::closeServer() {
	if (server_fd != INVALID_SOCKET) {
		closesocket(server_fd); // Close the server socket
	}
	WSACleanup(); // Clean up Winsock
}

void Server::handleDemo(SOCKET client_fd) {
	const char* response = "THIS IS DEMO";
	send(client_fd, response, strlen(response), 0);
}

void Server::sendErrorResponse(SOCKET client_fd, int statusCode) {
	const char* errorMessage = "404 Not Found";
	send(client_fd, errorMessage, strlen(errorMessage), 0);
}