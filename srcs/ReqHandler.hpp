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
	const std::string	_deleteReqHandler();
	std::string			_handleDirListing(std::string dirListing, std::string locIndex);
	void	_setEnvCgi(const HttpReqParsing& request, std::string& serverPath, std::vector<std::string>& env, std::string& cookies);
	void				_childCgi(int fd[2], const HttpReqParsing& request, std::vector<std::string>& env, char *args[]);
	void				_handleCgiReponse(int fd[2], std::multimap<std::string, std::string>& headersMap, std::string& body);

	Settings			_settings;
	const Location*		_reqLocation;
	std::string			_filePath;
	std::string			_fileName;
	std::string			_fullPath;

};

#endif
