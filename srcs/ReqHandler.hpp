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

	const std::string	_phpCgiHandler(const HttpReqParsing& request, const std::string& filePath);
	// const std::string	_pythonCgiHandler(const HttpReqParsing& request);
	const std::string	_getReqHandler(const HttpReqParsing& request);
	const std::string	_postReqHandler(const HttpReqParsing& request);
	const std::string	_deleteReqHandler(const HttpReqParsing& request);
	const std::string	_defaultHandler(const HttpReqParsing& request);

	// GetRequestHandler	getReqHandler;
	// PostRequestHandler	postReqHandler;
	// DefaultHandler		defaultHandler;
};

#endif
