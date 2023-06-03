
#include <algorithm>

#include "Server.hpp"
#include "HttpResponse.hpp"
#include "ReqHandler.hpp"
#include "utils.hpp"
#include "ErrorHandler.hpp"

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
		if (it == std::string::npos)
			break;
		it++;
		pos = it;
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
		if (ssport.str() != "80")
			host_conf_and_port = *it + ":" + ssport.str();
		else
			host_conf_and_port = *it;
		if (host_conf_and_port == host)
			return true;
	}
	return false;
}

Settings &Serv::hostMatchingConfigs(std::string request)
{
	std::string host = getHost(request);
	if (host.empty())
		std::cerr << "invalid request, host not found" << std::endl;
	for (std::vector<Settings>::iterator it = _settings.begin(); it != _settings.end(); it++)
	{
		if (hostMatching(host, miniSplit(it->server_name), it->port))
			return *it;
	}
	throw 421;
}

Serv::Serv() {}

Serv::~Serv() {}

void	Serv::settings_setter(std::vector<Settings> settings)
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

bool	Serv::maxBodyTooSmall(unsigned long contentLen, std::string request)
{
	std::stringstream ss;
	std::string host = getHost(request);
	for(std::vector<Settings>::iterator it = _settings.begin(); it != _settings.end(); it++)
	{
		ss.str(std::string());
		ss << it->port;
		for (std::vector<Settings>::iterator it = _settings.begin(); it != _settings.end(); it++)
		{
			if ((hostMatching(host, miniSplit(it->server_name), it->port))
					&& it->max_body && it->max_body < contentLen)
			{
				return false;
			}
		}
	}
	return true;
}

unsigned long my_stoul (std::string const& str, size_t *idx = 0, int base = 10) {
	char *endp;
	unsigned long value = strtoul(str.c_str(), &endp, base);
	if (endp == str.c_str()) {
		throw std::invalid_argument("my_stoul");
	}
	if (value == ULONG_MAX && errno == ERANGE) {
		throw std::out_of_range("my_stoul");
	}
	if (idx) {
		*idx = endp - str.c_str();
	}
	return value;
}

std::string Serv::chunkedBody(std::string req)
{
	std::string body;
	size_t chunkStartIndex = 0;
	while (true)
	{
		size_t chunkSizeEndIndex = req.find("\r\n", chunkStartIndex);
		if (chunkSizeEndIndex == std::string::npos)
			throw 400;

		std::string chunkSizeStr = req.substr(chunkStartIndex, chunkSizeEndIndex - chunkStartIndex);
		size_t chunkSize =my_stoul(chunkSizeStr, nullptr, 16);

		if (chunkSize == 0)
			break;

		size_t chunkDataStartIndex = chunkSizeEndIndex + 2;
		size_t chunkDataEndIndex = chunkDataStartIndex + chunkSize;
		if (chunkDataEndIndex > req.size())
			throw 400;

		body += req.substr(chunkDataStartIndex, chunkSize);
		chunkStartIndex = chunkDataEndIndex + 2;
	}
	return body;
}

bool	Serv::recvAll(int fd, std::string &request, std::string &body)
{
	char buffer[1024] = "";
	int bytesRead;
	bytesRead = recv(fd, buffer, sizeof(buffer), 0);
	if (bytesRead == -1)
	{
		close(fd);
		std::cerr << strerror(errno) << std::endl;
		return false;
	}
	if (bytesRead == 0)
	{
		close(fd);
		return false;
	}
	request.append(buffer, bytesRead);
	if (request.find("\r\n\r\n") != std::string::npos)
	{
		if (request.find("POST") != std::string::npos)
		{
			size_t bodyStartIndex = request.find("\r\n\r\n") + 4;
			body += request.substr(bodyStartIndex, std::string::npos);
			request.erase(bodyStartIndex, std::string::npos);
			if (request.find("Content-Length") != std::string::npos)
			{
				unsigned long contentLen = getContentLen(request);
				if (body.size() >= contentLen)
					return true;
			}
			else if (request.find("Transfer-Encoding: chunked") != std::string::npos)
			{
				if (body.find("0\r\n\r\n") != std::string::npos)
				{
					body = chunkedBody(body);
					if (!maxBodyTooSmall(body.size(), request))
						throw 413;
					return true;
				}
			}
			else
				throw 411;
		}
		else
			return true;
	}
	return false;
}

void	Serv::setBindAddrinfo()
{
	int fd;
	int bol = 1;
	struct addrinfo *p;
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
				err("server: socket");
				continue;
			}

			if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &bol,
					sizeof(int)) == -1)
				err("setsockopt");

			if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
				err("fcntl");
			}

			if (bind(fd, p->ai_addr, p->ai_addrlen) == -1) {
				close(fd);
				err("server: bind");
				continue;
			}
			sockfd[fd] = *it;
			break;
		}

		freeaddrinfo(_res);

		if (p == NULL)
		{
			err("server: failed to bind\n");
			exit(1);
		}
	}
}

int	Serv::noHeader(std::string request)
{
	if (request.find("HTTP") == std::string::npos)
		return 1;
	return (0);
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
			err("kevent");
	}

	handledEvents(kq);
}

void Serv::handledEvents(int kq)
{
	std::map<int, std::string> sendStrs;
	std::map<int, struct sRequest> recvStrs;
	std::map<int, struct kevent> timerEvents;
	struct kevent timerEv;
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
				EV_SET(&evSet, fd, EVFILT_TIMER, EV_DELETE, 0, 0, NULL);
				if (kevent(kq, &timerEv, 1, NULL, 0, NULL) == -1)
					Serv::err("kevent");
				EV_SET(&evSet, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
				if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
					Serv::err("kevent");
				close(fd);
				sendStrs.erase(fd);
				recvStrs.erase(fd);
			}
			if (sockfd.find(evList[i].ident) != sockfd.end()) {
				fd = accept(evList[i].ident, (struct sockaddr *)&addr, &socklen);
				if (fd == -1)
					Serv::err("accept");
				EV_SET(&evSet, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
				if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
					Serv::err("kevent");
				EV_SET(&timerEv, fd, EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, 10 * 1000, NULL);
				if (kevent(kq, &timerEv, 1, NULL, 0, NULL) == -1)
					Serv::err("kevent");
				timerEvents[fd] = timerEv;
				struct sRequest req;
				recvStrs[fd] = req;
			}
			if (evList[i].filter == EVFILT_READ) {
				fd = evList[i].ident;
				if (recvStrs.find(fd) != recvStrs.end() && isAvailable(fd)) 
				{
					std::string response;
					try 
					{
						if (!recvAll(fd, recvStrs[fd].request, recvStrs[fd].body))
							continue ;
						if (noHeader(recvStrs[fd].request))
							continue;
						Settings sett = hostMatchingConfigs(recvStrs[fd].request);
						HttpReqParsing hReq(recvStrs[fd].request, recvStrs[fd].body, sett);
						ReqHandler reqHandler(sett);
						response = reqHandler.handleRequest(hReq);
					} 
					catch (int errorCode)
					{
						try {
							Settings sett = hostMatchingConfigs(recvStrs[fd].request);
							ErrorHandler errHandler(errorCode, sett.error_pages, sett);
							errHandler.generateBody();
							HttpResponse errRes(errorCode, errHandler.getBody());
							response = errRes.toString();
						}
						catch (int errorCode)
						{
							ErrorHandler errHandler(errorCode);
							errHandler.generateBody();
							HttpResponse errRes(errorCode, errHandler.getBody());
							response = errRes.toString(); 
						}
					}
					sendStrs[fd] = response;
					recvStrs[fd].request = "";
					recvStrs[fd].body = "";
					EV_SET(&evSet, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
					if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
						Serv::err("kevent");
					EV_SET(&evSet, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
					if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
						Serv::err("kevent");
				}
			}
			if (evList[i].filter == EVFILT_WRITE)
			{
				fd = evList[i].ident;
				if (sendStrs.find(fd) == sendStrs.end() || sendStrs[fd].empty())
					continue ;
				if (!isAvailable(fd))
					continue ;
				if (sendStrs[fd].length() >= 1000) 
				{
					int n = send(fd, sendStrs[fd].c_str(), 1000, 0);
					if (n == -1)
					{
						std::cerr << strerror(errno) << std::endl;
						close(fd);
						continue ;
					}
					sendStrs[fd].erase(0, n);
				}
				else
				{
					int n = send(fd, sendStrs[fd].c_str(), sendStrs[fd].length(), 0);
					if (n == -1)
					{
						std::cerr << strerror(errno) << std::endl;
						close(fd);
						continue ;
					}
					sendStrs[fd].erase(0, n);
					if (sendStrs[fd].empty())
						sendStrs.erase(fd);
					close(fd);
				}
			}
			if (evList[i].filter == EVFILT_TIMER) {
				fd = evList[i].ident;
				if (timerEvents.find(fd) != timerEvents.end())
				{
					timerEvents.erase(fd);
					close(fd);
				}
			}
		}
	}
}
