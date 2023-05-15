#include <iostream>

#ifndef ROUTER_HPP
# define ROUTER_HPP

class Router {

public:

	Router();
	~Router();


private:

	Router();
	Router(const Router& src);
	Router& operator=(const Router& rhs);


	// CGI

	// GetRequestHandler	getReqHandler;
	// PostRequestHandler	postReqHandler;
	// DefaultHandler		defaultHandler;
};

#endif
