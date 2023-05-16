#include "HttpReqParsing.hpp"
#include "HttpResponse.hpp"
#include "utils.hpp"
#include "ReqHandler.hpp"
#include <algorithm>

#include "Server.hpp"

Serv::Serv() {}

Serv::~Serv() {}

// void	serv::setSocket()

const void	Serv::port_setter(const int port)
{
	_port = port;
}

const void	Serv::nevents_setter(const int nevents)
{
	_nevents = nevents;
}


void	Serv::err(std::string msg)
{
	std::cerr << msg << std::endl;
	exit(1);
}

void	Serv::sendall(int fd, std::string msg)
{

}

void	Serv::setAddrinfo()
{
	int status;
	struct addrinfo hints;
	
	std::memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; //whatever ip4 or ip6
	hints.ai_socktype = SOCK_STREAM; // TCP stream socket (2way connection)
	hints.ai_flags = AI_PASSIVE | O_NONBLOCK; //localhost ip | nonblocking socket?

	if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
		std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
		exit(1);
	}
}

void	Serv::bindSocket()
{
	int bol = 1;
	struct addrinfo *p;

	for(p = res; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			std::cerr << "server: socket" << std::endl;
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &bol,
				sizeof(int)) == -1)
			Serv::err("setsockopt");

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			std::cerr << "server: bind" << std::endl;
			continue;
		}

		break;
	}

	freeaddrinfo(res); // free structure

	if (p == NULL)
		Serv::err("server: failed to bind\n");
}

void	Serv::srvListen()
{
	if (listen(sockfd, BACKLOG) == -1)
		Serv::err("listen");
}

void Serv::setEvent()
{
	int kq;
	struct kevent evSet;

	kq = kqueue();

	EV_SET(&evSet, sockfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
		Serv::err("kevent");

	handledEvents(kq);
}

void Serv::handledEvents(int kq)
{
	std::deque<int> uc;
	std::deque<int>::iterator itr;
	char request[1024];
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
				std::cout << "disconnect\n" << std::endl;
				fd = evList[i].ident;
				EV_SET(&evSet, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
				if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
					Serv::err("kevent");
				
				itr = find(uc.begin(), uc.end(), fd);
				if (itr != uc.end()) {
					uc.erase(itr);
					for (std::deque<int>::iterator iter = uc.begin(); iter != uc.end(); ++iter)
						std::cout << *iter << std::endl;
				}
				
				// Close the file descriptor
				close(fd);
			}
			else if (evList[i].ident == sockfd) {
				fd = accept(evList[i].ident, (struct sockaddr *)&addr, &socklen);
				if (fd == -1)
					Serv::err("accept");
				
				itr = find(uc.begin(), uc.end(), fd);
				if (itr == uc.end()) {
					uc.push_back(fd);
					EV_SET(&evSet, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
					if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
						Serv::err("kevent");
					std::cout << "welcome" << std::endl;
				} else {
					std::cout << "connection refused" << std::endl;
					// Close the file descriptor
					close(fd);
				}
				
				// Print the active connections
				for (std::deque<int>::iterator iter = uc.begin(); iter != uc.end(); ++iter)
					std::cout << "hi " << *iter << std::endl;
			}
			else if (evList[i].filter == EVFILT_READ) {
				fd = evList[i].ident;
				recv(fd, request, sizeof(request), 0); // use recvall
				HttpReqParsing hReq(request);

				ReqHandler reqHandler;
				std::string response = reqHandler.handleRequest(hReq);
				send(fd, response.c_str(), response.size(), 0);
				std::cout << "sending" << std::endl;
				
				// Clear the connection deque and close the file descriptor
				uc.clear();
				close(fd);
			}
		}
	}
}