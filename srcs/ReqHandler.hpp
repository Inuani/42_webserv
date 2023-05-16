#include <iostream>
#include "HttpReqParsing.hpp"
#include "HttpResponse.hpp"

#ifndef REQHANDLER_HPP
# define REQHANDLER_HPP

class ReqHandler {

public:

	ReqHandler();
	~ReqHandler();

	const std::string	handleRequest(const HttpReqParsing& request);

private:

	ReqHandler(const ReqHandler& src);
	ReqHandler& operator=(const ReqHandler& rhs);

	// CGI
	const std::string	getReqHandler(const HttpReqParsing& request);
	const std::string	postReqHandler(const HttpReqParsing& request);
	const std::string	deleteReqHandler(const HttpReqParsing& request);
	const std::string	defaultHandler(const HttpReqParsing& request);

	// GetRequestHandler	getReqHandler;
	// PostRequestHandler	postReqHandler;
	// DefaultHandler		defaultHandler;
};

#endif
