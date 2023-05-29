
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
#include <vector>
#include <map>
#include <set>

#include "Settings.h"
#include "HttpReqParsing.hpp"

#define PORT "8080"
#define BACKLOG 1024
#define NEVENTS 4096

class Serv
{
	private:
		std::string _request;
		std::string _body;
		struct addrinfo *_res;
		std::vector<Settings> _settings;
		std::map<int, Settings> sockfd;
		//std::string _response;

	public:
		Serv();
		~Serv();
		Serv(Settings settings);

		void		err(std::string msg);
		void		setBindAddrinfo();
		void		srvListen();
		void		setEvent();
		void		handledEvents(int kq);
		void		recvAll(int fd);
		bool		maxBodyTooSmall(int);

		std::vector<std::string> miniSplit(std::string toSplit);
		bool hostMatching(std::string host, std::vector<std::string> hosts_conf, int port);
		Settings &hostMatchingConfigs();
		std::string	getHost(std::string);
		const void	settings_setter(std::vector<Settings> settings);
};

#endif
