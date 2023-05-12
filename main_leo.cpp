#include "Server.hpp"

int main(int argc, char *argv[])
{
	Serv s;
	std::cout << "waiting for connections..." << std::endl;

	s.setAddrinfo();
	s.bindSocket();
	s.srvListen();
	// s.setSignals();
	s.setEvent();

	std::cout << "hi";
}