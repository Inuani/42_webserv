#include <iostream>
#include "HttpReqParsing.hpp"
#include "HttpResponse.hpp"
#include "Settings.h"
#include "utils.hpp"

#ifndef REQHANDLER_HPP
# define REQHANDLER_HPP

class ReqHandler {

public:

	ReqHandler();
	ReqHandler(const Settings& settings);
	~ReqHandler();
	const std::string	handleRequest(const HttpReqParsing& request);

private:

	ReqHandler(const ReqHandler& src);
	ReqHandler& operator=(const ReqHandler& rhs);

	const std::string	_cgiHandler(const HttpReqParsing& request);
	const std::string	_getReqHandler(const HttpReqParsing& request);
	const std::string	_postReqHandler(const HttpReqParsing& request);
	const std::string	_deleteReqHandler(const HttpReqParsing& request);
	const std::string	_defaultHandler(const HttpReqParsing& request);

	std::string handleDirListing(std::string dirListing, std::string locIndex);
	Settings			_settings;
	const Location*		_reqLocation;
	std::string			_filePath;
	std::string			_fileName;
	std::string			_fullPath;

	// GetRequestHandler	getReqHandler;
	// PostRequestHandler	postReqHandler;
	// DefaultHandler		defaultHandler;
};

#endif
