
#ifndef WEBSERV
# define WEBSERV

#include <iostream>
#include <fstream>
#include <sstream>

#include <sys/event.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <unistd.h>
#include <signal.h>
#include <string.h>

#include <cstring>
#include <cstdio>

#include <deque>

#define PORT "8080"
#define BACKLOG 10
#define NEVENTS 1024

class Serv
{
	private:
		//std::deque<int> uc; //useful here?
		int sockfd;
		struct addrinfo *res;
		int _port;
		int _nevents;

	public:
		Serv();
		~Serv();
		void	err(std::string msg);
		void	setAddrinfo();
		void	bindSocket();
		void	srvListen();
		void	setEvent();
		void	handledEvents(int kq);
		void	sendall(int fd, std::string msg);

		const void	port_setter(int port);
		const void	nevents_setter(int nevents);
};

#endif
