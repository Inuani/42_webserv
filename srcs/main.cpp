#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/event.h>
#include <sys/time.h>
#include <netdb.h>
#include <string.h>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "utils.hpp"

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
            struct sockaddr_storage theirAddr; // generel purpose socket address struct that stock client address
            // when connected to the server
            socklen_t addr_size = sizeof theirAddr;
            int clientSockFd = accept(serverFd, (struct sockaddr *)&theirAddr, &addr_size);
            // accept extracts first connection request on the queue of pending connections
            // for the listening socket (server_fd) creates a new connected socket,
            // and returns a new file descriptor referring to that socket.

            std::string request;
            char buffer[1024];
            std::string last4Chars;
            ssize_t bytesRead;

            while ((bytesRead = read(clientSockFd, buffer, sizeof(buffer))) > 0) {
                request.append(buffer, bytesRead);
    
                last4Chars.append(buffer, bytesRead);
                if (last4Chars.size() > 4)
                last4Chars.erase(0, last4Chars.size() - 4);
                if (last4Chars == "\r\n\r\n")
                    break;
                }
                

            // char request[30000] = {0};
            // read(clientSockFd, request, 30000);

            // std::cout << request << std::endl;

            HttpRequest hReq(request);

            std::string filePath = hReq.getUri();
            if (hReq.getUri() == "/")
                filePath = "srcs/index.html";
            std::string body = readFileContent(filePath);

            HttpResponse hRes(200, body, "text/html");
            std::string response = hRes.toString();
            std::cout << response << std::endl;

            // const char *hello = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 12\r\n\r\nHello world!";
            // write(clientSockFd, hello, strlen(hello));
            write(clientSockFd, response.c_str(), response.size());
            // std::cout << "------------------Hello message sent-------------------\n";

            close(clientSockFd);
        }
    }
    return 0;
}
