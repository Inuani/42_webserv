
#include <algorithm>

#include "Server.hpp"
#include "HttpResponse.hpp"
#include "ReqHandler.hpp"
#include "utils.hpp"

std::vector<std::string> Serv::miniSplit(std::string toSplit)
{
	std::vector<std::string> split;
	std::size_t it = 0;
	std::size_t pos = 0;
	while (1)
	{
		it = toSplit.find(",", it);
		std::string token = toSplit.substr(pos, it);
		split.push_back(token);
		pos = it;
		if (it == std::string::npos)
			break;
		it++;

	}
	return split;
}

bool Serv::hostMatching(std::string host, std::vector<std::string> hosts_conf, int port)
{
	std::string host_conf_and_port;
	std::stringstream ssport;
	ssport << port;
	for (std::vector<std::string>::iterator it = hosts_conf.begin(); it != hosts_conf.end(); it++)
	{
		host_conf_and_port = *it + ":" + ssport.str();
		if (host_conf_and_port == host)
			return true;
	}
	return false;
}

Settings &Serv::hostMatchingConfigs(void)
{
	std::string host = getHost(_request); //check if host empty?
	for (std::vector<Settings>::iterator it = _settings.begin(); it != _settings.end(); it++)
	{
		if (hostMatching(host, miniSplit(it->server_name), it->port))
			return *it;
	}
	return _settings.front();
}

Serv::Serv() {}

Serv::~Serv() {}
// void	serv::setSocket()

const void	Serv::settings_setter(std::vector<Settings> settings)
{
	_settings = settings;
}

void	Serv::err(std::string msg)
{
	std::cerr << msg << std::endl;
}

bool isAvailable(int fd)
{
	ssize_t bytesRead;
	char buffer;
	bytesRead = read(fd, &buffer, 0);
		if (bytesRead < 0)
			return false;
	return true;
}

bool	Serv::sendall(int fd, std::string msg)
{
	size_t total = 0;
	size_t len = msg.size();
	ssize_t n;

	while (total < len) {
		if (!isAvailable(fd))
			return false;
		n = send(fd, msg.c_str() + total, len - total, 0);
		if (n == -1)
		{
			std::cout << strerror(errno) << n << std::endl; //...
			continue;
		}
		total += n;
	}
	if (total != len) {
		std::cout << "Error: sendall - Incomplete data sent" << std::endl;
		return false;
	}
	return true;
}

std::string	Serv::getHost(std::string header)
{
	std::istringstream ssreq(header);
	std::string host;
	while(ssreq >> host)
	{
		if (host == "Host:")
		{
			ssreq >> host;
			return (host);
		}
	}
	return "";
}

bool	Serv::maxBodyTooSmall(int contentLen)
{
	std::stringstream ss;
	std::string host = getHost(_request);
	for(std::vector<Settings>::iterator it = _settings.begin(); it != _settings.end(); it++)
	{
		ss << it->port;
		std::cout << "HOST NAME " << it->server_name + ":" + ss.str() << std::endl;
		if (it->server_name.append(":" + ss.str()) == host
				&& it->max_body && it->max_body < contentLen)
		{
			return false;
		}
	}
	return true;
}

//this code is ugly AF
void	Serv::recvAll(int fd)
{
	_request = "";
	_body = "";
	char buffer[1024];
	ssize_t bytesRead;
	while ((bytesRead = recv(fd, buffer, sizeof(buffer), 0)) > 0) {
		_request.append(buffer, bytesRead);
		if (_request.find("\r\n\r\n") != std::string::npos)
			break;
	}

	if (bytesRead == -1) {
		std::cerr << "Error in recv: " << strerror(errno) << std::endl;
		if (isAvailable(fd))
			close(fd);
		return;
	}

	if (_request.find("Content-Length") != std::string::npos)
	{
		//split the _request to get the _body
		if (_request.find("\r\n\r\n") + 4 != std::string::npos)
		{
			_body = _request.substr(_request.find("\r\n\r\n") + 4, std::string::npos);
			_request.erase(_request.find("\r\n\r\n", + 4), std::string::npos);
		}
		int contentLength = getContentLen(_request);
		if (!maxBodyTooSmall(contentLength))
		{
			exit(1);
			std::cout << std::endl << std::endl << std::endl << "ALAIDDDD " << std::endl << std::endl << std::endl << std::endl;
		}
		// Maybe use || instead of &&
		while (_body.size() < contentLength && (bytesRead = recv(fd, buffer, sizeof(buffer), 0)) > 0)
		{
			_body.append(buffer, bytesRead);
		}
	}
}

void	Serv::setBindAddrinfo()
{
	int fd;
	int bol = 1;
	struct addrinfo *p;
	int i = 0;
	int status;
	struct addrinfo hints;
	
	std::memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; //whatever ip4 or ip6
	hints.ai_socktype = SOCK_STREAM; // TCP stream socket (2way connection)
	hints.ai_flags = AI_PASSIVE | O_NONBLOCK; //localhost ip | nonblocking socket?

	for(std::vector<Settings>::iterator it = _settings.begin(); it != _settings.end(); it++)
	{
		std::stringstream ss;
		ss << it->port;
		std::string port = ss.str();
		if ((status = getaddrinfo(NULL, port.c_str(), &hints, &_res)) != 0) {
			std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
			exit(1);
		}
		for(p = _res; p != NULL; p = p->ai_next) {
			if ((fd = socket(p->ai_family, p->ai_socktype,
					p->ai_protocol)) == -1) {
				std::cerr << "server: socket" << std::endl;
				continue;
			}

			if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &bol,
					sizeof(int)) == -1)
				Serv::err("setsockopt");

			if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
				Serv::err("fcntl");
			}

			if (bind(fd, p->ai_addr, p->ai_addrlen) == -1) {
				close(fd);
				std::cerr << "server: bind" << std::endl;
				continue;
			}
			sockfd[fd] = *it;
			break;
		}

		freeaddrinfo(_res); // free structure

		if (p == NULL)
		{
			Serv::err("server: failed to bind\n");
			//exit(1);
		}
	}
}

void	Serv::srvListen()
{
	for(std::map<int, Settings>::iterator it = sockfd.begin(); it != sockfd.end(); it++)
	{
		if (listen(it->first, BACKLOG) == -1)
			Serv::err("listen");
	}
}

void Serv::setEvent()
{
	int kq;
	struct kevent evSet;

	kq = kqueue();

	for(std::map<int, Settings>::iterator it = sockfd.begin(); it != sockfd.end(); it++)
	{
		EV_SET(&evSet, it->first, EVFILT_READ, EV_ADD, 0, 0, NULL);
		if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
			Serv::err("kevent");
	}

	handledEvents(kq);
}


void Serv::handledEvents(int kq)
{
	//std::map<int, Settings>::iterator it;
	std::deque<int>::iterator itr;
	struct kevent evList[NEVENTS];
	struct kevent evSet;
	struct sockaddr_storage addr;
	socklen_t socklen = sizeof(addr);
	int nev, i;
	int fd = 0;

	while (true) {
		nev = kevent(kq, NULL, 0, evList, NEVENTS, NULL);
		if (nev < 0)
			Serv::err("kevent");
		
		for (i = 0; i < nev; i++) {
			if (evList[i].flags & EV_EOF) {
				fd = evList[i].ident;
				EV_SET(&evSet, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
				if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
					Serv::err("kevent");
				close(fd);
			}
			else if (sockfd.find(evList[i].ident) != sockfd.end()) {
				fd = accept(evList[i].ident, (struct sockaddr *)&addr, &socklen);
				if (fd == -1)
					Serv::err("accept");
				EV_SET(&evSet, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
				if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
					Serv::err("kevent");
			}
			else if (evList[i].filter == EVFILT_READ) {
				fd = evList[i].ident;
				recvAll(fd);
				try {
					HttpReqParsing hReq(_request, _body, hostMatchingConfigs());
					ReqHandler reqHandler(hostMatchingConfigs());
					std::string response = reqHandler.handleRequest(hReq);
					sendall(fd, response);
				} catch (const std::runtime_error& e) {
					std::cout << e.what() << std::endl;

					int errorStatus;
					std::istringstream errS(e.what());
					if (!(errS >> errorStatus)) {
						errorStatus = 404;
					}
					std::string errorBody = e.what();
					HttpResponse errRes(errorStatus, errorBody);
					std::string response = errRes.toString();
					sendall(fd, response);
				} 
				if (isAvailable(fd))
					close(fd);
			}
		}
	}
}
