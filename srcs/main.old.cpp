#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/event.h>
#include <sys/time.h>
#include <netdb.h>
#include <string.h>
#include "HttpReqParsing.hpp"
#include "HttpResponse.hpp"
#include "utils.hpp"
#include "ReqHandler.hpp"
#include <fstream>

#define PORT "8080"

int main() {
    struct addrinfo hints, *res;
    int serverFd;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // specify that we want to use our own IP address
    
    getaddrinfo(NULL, PORT, &hints, &res);
    // getaddrinfo() returns a list of addresses,
    // which is a list of possible combinations of IP addresses, port numbers, and protocols
    // that satisfy the criteria you've specified in the hints struct.

    // Creating socket file descriptor
    if ((serverFd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == 0) {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    int bol = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &bol,
				sizeof(int)) == -1) {
        perror("setsockopt Error");
        exit(EXIT_FAILURE);
    }

	// socket is associated with the address and port from struct sockaddr_in
    if (bind(serverFd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("In bind");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(res); // free the linked-list

	// make the socket passive so that it can accept connection with accept()
    if (listen(serverFd, 10) < 0) {
        perror("In listen");
        exit(EXIT_FAILURE);
    }

    int kq = kqueue(); // new kernel queue created.
    // now events can be registered to this kqueue using kevent() function
    struct kevent change_list; // structure used to describe an event.
    // It includes fields for the identifier of the object to monitor
    // (like a file descriptor for a socket or file),
    // the type of event to monitor, action to take (like adding or removing the event), etc.
    EV_SET(&change_list, serverFd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    // serverFd : identifier for the event
    // EVFILT_READ : event filter. specifies what type of event to monitor
    // Action to take. EV_ADD means adding this event to the kqueue

    // setting up a kqueue to monitor the socket (described by serverFd) for incoming data to read.
    // When data arrives and the socket is ready for reading, an event will be triggered.

    while(1) {
        struct kevent event;
        kevent(kq, &change_list, 1, &event, 1, NULL); // applies changes kqueue and retrieves events.
        // It's applying the changes specified in change_list and retrieving any pending events into event.
        if (event.filter == EVFILT_READ) // is the event a read event ?
        {
            struct sockaddr_storage theirAddr; // general purpose socket address struct that stock client address
            // when connected to the server
            socklen_t addr_size = sizeof theirAddr;
            int clientSockFd = accept(serverFd, (struct sockaddr *)&theirAddr, &addr_size);
            // accept extracts first connection request on the queue of pending connections
            // for the listening socket (server_fd) creates a new connected socket,
            // and returns a new file descriptor referring to that socket.

            std::string request;
            std::string body;
            char buffer[1024];
            ssize_t bytesRead;
            while ((bytesRead = read(clientSockFd, buffer, sizeof(buffer))) > 0) {
            request.append(buffer, bytesRead);
            if (request.find("\r\n\r\n") != std::string::npos)
                break;
            }

            if (request.find("Content-Length") != std::string::npos)
            {
                //split the request to get the body
                if (request.find("\r\n\r\n") + 4 != std::string::npos)
                {
                    body = request.substr(request.find("\r\n\r\n") + 4, std::string::npos);
                    request.erase(request.find("\r\n\r\n", + 4), std::string::npos);
                }
                int contentLength = getContentLen(request);
                // Maybe use || instead of &&
                while (body.size() < contentLength && (bytesRead = read(clientSockFd, buffer, sizeof(buffer))) > 0)
                {
                    body.append(buffer, bytesRead);
                }
                //HttpReqParsing hReq(request, body);
            }
            HttpReqParsing hReq(request, body);
            // std::cout << std::endl;
            // std::cout << "<--------------- body --------------->" << std::endl;
            // // std::cout << body << std::endl;
            // std::cout << "<--------------- end body --------------->" << std::endl;
            
            //std::ofstream file("test.txt");
            //file << body;
            ReqHandler  reqHandler;
            std::string response = reqHandler.handleRequest(hReq);
            // std::cout << response << std::endl;
            write(clientSockFd, response.c_str(), response.size());
            close(clientSockFd);
        }
    }
    return 0;
}

int main() {
    struct addrinfo hints, *res;
    int serverFd;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // specify that we want to use our own IP address
    getaddrinfo(NULL, PORT, &hints, &res);
    if ((serverFd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == 0) {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    int bol = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &bol,
				sizeof(int)) == -1) {
        perror("setsockopt Error");
        exit(EXIT_FAILURE);
    }
    if (bind(serverFd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("In bind");
        exit(EXIT_FAILURE);}
    freeaddrinfo(res); // free the linked-list
    if (listen(serverFd, 10) < 0) {
        perror("In listen");
        exit(EXIT_FAILURE);}
    int kq = kqueue(); // new kernel queue created.

    struct kevent change_list; // structure used to describe an event.
    EV_SET(&change_list, serverFd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    while(1) {
        struct kevent event;
        kevent(kq, &change_list, 1, &event, 1, NULL); // applies changes kqueue and retrieves events.t.
        if (event.filter == EVFILT_READ) // is the event a read event ?
        {
            struct sockaddr_storage theirAddr; // general purpose socket address struct that stock client address

            socklen_t addr_size = sizeof theirAddr;
            int clientSockFd = accept(serverFd, (struct sockaddr *)&theirAddr, &addr_size);
            std::string request;
            std::string body;
            char buffer[1024];
            ssize_t bytesRead;
            while ((bytesRead = read(clientSockFd, buffer, sizeof(buffer))) > 0) {
            request.append(buffer, bytesRead);
            if (request.find("\r\n\r\n") != std::string::npos)
                break;
            }
            if (request.find("Content-Length") != std::string::npos)
            {

                if (request.find("\r\n\r\n") + 4 != std::string::npos)
                {
                    body = request.substr(request.find("\r\n\r\n") + 4, std::string::npos);
                    request.erase(request.find("\r\n\r\n", + 4), std::string::npos);
                }
                int contentLength = getContentLen(request);
                while (body.size() < contentLength && (bytesRead = read(clientSockFd, buffer, sizeof(buffer))) > 0)
                {
                    body.append(buffer, bytesRead);
                }
            }
            HttpReqParsing hReq(request, body);
            ReqHandler  reqHandler;
            std::string response = reqHandler.handleRequest(hReq);
            write(clientSockFd, response.c_str(), response.size());
            close(clientSockFd);
        }
    }
    return 0;
}