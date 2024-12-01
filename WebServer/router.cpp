#include "router.hpp"

void Router::addRoute(const std::string& method, const std::string& path, Handler handler) {
	routes[method + path] = handler;
}

bool Router::handleRequest(const std::string& method, const std::string& path, SOCKET client_fd) {
	std::string key = method + path;

	if (routes.find(key) != routes.end()) {
		routes[key](client_fd);
		return true;
	}
	
	return false;
}