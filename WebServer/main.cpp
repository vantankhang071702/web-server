#include <iostream>
#include "server.hpp"

int main() {
    std::cout << "Starting Web Server..." << std::endl;

    // Initialize the server
    Server myServer;

    // Start the server
    myServer.start();  // You can pass the port number here

    return 0;
}