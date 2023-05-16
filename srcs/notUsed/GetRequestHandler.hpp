#include <iostream>

#ifndef GETREQUESTHANDLER_HPP
# define GETREQUESTHANDLER_HPP

class GetRequestHandler {

public:

	GetRequestHandler();
	GetRequestHandler(const GetRequestHandler& src);
	~GetRequestHandler();

	GetRequestHandler& operator=(const GetRequestHandler& rhs);

private:

};

#endif
