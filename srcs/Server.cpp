
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

void	Serv::sendall(int fd, std::string msg)
{
	int total = 0;
	int len = msg.size();
	int n;

	while(total < len) {
		n = send(fd, msg.c_str(), msg.size(), 0);
		if (n == -1) { break; }
		total += n;
		msg.erase(0, n);
	}
	if (n == -1)
		std::cout << "Error: sendall" << std::endl;
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
	else if (_request.find("Transfer-Encoding: chunked") != std::string::npos)
	{

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
	hints.ai_flags = AI_PASSIVE; //localhost ip

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
				activeSockets.erase(fd);
				close(fd);
			}
			else if (sockfd.find(evList[i].ident) != sockfd.end()) {
				fd = accept(evList[i].ident, (struct sockaddr *)&addr, &socklen);
				if (fd == -1)
					Serv::err("accept");
				EV_SET(&evSet, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
				if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
					Serv::err("kevent");
				activeSockets.insert(fd);
			}
			else if (evList[i].filter == EVFILT_READ) {
				fd = evList[i].ident;
				if (activeSockets.find(fd) == activeSockets.end()) {
					close(fd);
					continue;
				}
				recvAll(fd);
				// if (hostMatchingConfigs())
				// 	std::cout << "yey" << std::endl;
				// else
				// 	std::cout << "nay" << std::endl;

				HttpReqParsing hReq(_request, _body);
				ReqHandler reqHandler;
				std::string response = reqHandler.handleRequest(hReq);
				sendall(fd, response);
				close(fd);
			}
		}
	}
}


// void Serv::handledEvents(int kq)
// {
// 	//std::map<int, Settings>::iterator it;
// 	std::deque<int> uc;
// 	std::deque<int>::iterator itr;
// 	char _request[1024];
// 	struct kevent evList[NEVENTS];
// 	struct kevent evSet;
// 	struct sockaddr_storage addr;
// 	socklen_t socklen = sizeof(addr);
// 	int nev, i;
// 	int fd = 0;

// 	while (true) {
// 		nev = kevent(kq, NULL, 0, evList, NEVENTS, NULL);
// 		if (nev < 0)
// 			Serv::err("kevent");
		
// 		for (i = 0; i < nev; i++) {
// 			if (evList[i].flags & EV_EOF) {
// 				std::cout << "disconnect\n" << std::endl;
// 				fd = evList[i].ident;
// 				EV_SET(&evSet, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
// 				if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
// 					Serv::err("kevent");
				
// 				itr = find(uc.begin(), uc.end(), fd);
// 				if (itr != uc.end()) {
// 					uc.erase(itr);
// 					for (std::deque<int>::iterator iter = uc.begin(); iter != uc.end(); ++iter)
// 						std::cout << *iter << std::endl;
// 				}
// 				// Close the file descriptor
// 				close(fd);
// 			}
// 			else if (sockfd.find(evList[i].ident) != sockfd.end()) {
// 				fd = accept(evList[i].ident, (struct sockaddr *)&addr, &socklen);
// 				if (fd == -1)
// 					Serv::err("accept");
				
// 				itr = find(uc.begin(), uc.end(), fd);
// 				if (itr == uc.end()) {
// 					uc.push_back(fd);
// 					EV_SET(&evSet, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
// 					if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
// 						Serv::err("kevent");
// 					std::cout << "welcome" << std::endl;
// 				} else {
// 					std::cout << "connection refused" << std::endl;
// 					// Close the file descriptor
// 					close(fd);
// 				}
				
// 				// Print the active connections
// 				// for (std::deque<int>::iterator iter = uc.begin(); iter != uc.end(); ++iter)
// 				// 	std::cout << "hi " << *iter << std::endl;
// 			}
// 			else if (evList[i].filter == EVFILT_READ) {
// 				memset(_request, 0, sizeof(_request));
// 				fd = evList[i].ident;
// 				recv(fd, _request, sizeof(_request), 0); // use recvall
// 				//std::cout << "adsadasdasdasdssadsadsaasd\n" << _request << "adsadasdasdasdssadsadsaasd\n" << std::endl;
// 				HttpReqParsing hReq(_request);

// 				ReqHandler reqHandler;
// 				std::string response = reqHandler.handleRequest(hReq);
// 				sendall(fd, response);
// 				//send(fd, response.c_str(), response.size(), 0);
// 				std::cout << "sending" << std::endl;
				
// 				// Clear the connection deque and close the file descriptor
// 				uc.clear();
// 				close(fd);
// 			}
// 		}
// 	}
// }

