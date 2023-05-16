#include "HttpReqParsing.hpp"
#include <iostream>
#include <sstream>
#include <map>

HttpReqParsing::HttpReqParsing(const std::string& strHttpRequest) {
	std::istringstream parseStream(strHttpRequest);
	std::string line;

	std::getline(parseStream, line);
	std::istringstream requestStream(line);
	requestStream >> _method >> _uri >> _version;

	std::cout << _method << " " << _uri << " " << _version << std::endl;

	while (std::getline(parseStream, line) && line != "\r") {
		std::istringstream headerStream(line);
		std::string key, value;
		std::getline(headerStream, key, ':');
		headerStream >> value;
		// headerStream.ignore(1); //caution
		// std::getline(headerStream, value);
		_headers[key] = value;
		std::cout << key << " : " << value <<std::endl;
	}

	while (std::getline(parseStream, line)) {
		_body.append(line + "\n");
	}
	std::cout << _body << std::endl;

}

HttpReqParsing::~HttpReqParsing() {}

const std::string&	HttpReqParsing::getMethod() const {
	return _method;
}

const std::string&	HttpReqParsing::getUri() const {
	return _uri;
}

const std::string&	HttpReqParsing::getVersion() const {
	return _version;
}

const std::map<std::string, std::string>&	HttpReqParsing::getheaders() const {
	return _headers;
}

const std::string&	HttpReqParsing::getBody() const {
	return _body;
}

HttpReqParsing::HttpReqParsing() {}

HttpReqParsing::HttpReqParsing(const HttpReqParsing &src) {
	*this = src;
}

HttpReqParsing&	HttpReqParsing::operator=(const HttpReqParsing& rhs) {
	(void)rhs;
	return *this;
}

