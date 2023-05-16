#include "Server.hpp"
#include "confparser.cpp"

int main(int argc, char *argv[])
{
	//main_loop();
	Serv s;
	// std::cout << "waiting for connections..." << std::endl;



	s.setAddrinfo();
	s.bindSocket();
	s.srvListen();
	s.setEvent();

	// std::cout << "hi";
}
