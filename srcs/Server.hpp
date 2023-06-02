
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

struct sRequest
{
	std::string request;
	std::string body;
};

class Serv
{
	private:
		//std::string _request;
		std::string _body;
		struct addrinfo *_res;
		std::vector<Settings> _settings;
		std::map<int, Settings> sockfd;
		int noHeader(std::string request);
		//std::string _response;

	public:
		Serv();
		~Serv();
		Serv(Settings settings);

		std::string chunkedBody(std::string req);
		void		err(std::string msg);
		void		setBindAddrinfo();
		void		srvListen();
		void		setEvent();
		void		handledEvents(int kq);
		bool		recvAll(int fd, std::string &, std::string &);
		bool		maxBodyTooSmall(unsigned long, std::string);

		std::vector<std::string> miniSplit(std::string toSplit);
		bool hostMatching(std::string host, std::vector<std::string> hosts_conf, int port);
		Settings &hostMatchingConfigs(std::string);
		std::string	getHost(std::string);
		void	settings_setter(std::vector<Settings> settings);
};

#endif
