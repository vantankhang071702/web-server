#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <functional>
#include <unordered_map>
#include <string>
#include <winsock2.h>

class Router {
public:
	using Handler = std::function<void(SOCKET)>;
	void addRoute(const std::string& method, const std::string& path, Handler hander);
	bool handleRequest(const std::string& method, const std::string& path, SOCKET client_fd);

private:
	std::unordered_map<std::string, Handler> routes;
};

#endif // ROUTER_HPP