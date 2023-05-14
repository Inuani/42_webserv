#include "HttpRequest.hpp"
#include <iostream>
#include <sstream>
#include <map>

HttpRequest::HttpRequest(const std::string& strHttpRequest) {
	std::istringstream parseStream(strHttpRequest);
	std::string line;

	std::getline(parseStream, line);
	std::istringstream requestStream(line);
	requestStream >> _method >> _uri >> _version;

	// std::cout << _method << _uri << _version << std::endl;

	while (std::getline(parseStream, line) && line != "\r") {
		std::istringstream headerStream(line);
		std::string key, value;
		std::getline(headerStream, key, ':');
		headerStream >> value;
		// headerStream.ignore(1); //caution
		// std::getline(headerStream, value);
		_headers[key] = value;
		// std::cout << key << " : " << value <<std::endl;
	}

	while (std::getline(parseStream, line)) {
		_body.append(line + "\n");
	}
}

HttpRequest::~HttpRequest() {}

const std::string&	HttpRequest::getMethod() const {
	return _method;
}

const std::string&	HttpRequest::getUri() const {
	return _uri;
}

const std::string&	HttpRequest::getVersion() const {
	return _version;
}

const std::map<std::string, std::string>&	HttpRequest::getheaders() const {
	return _headers;
}

const std::string&	HttpRequest::getBody() const {
	return _body;
}

HttpRequest::HttpRequest() {}

HttpRequest::HttpRequest(const HttpRequest &src) {
	*this = src;
}

HttpRequest&	HttpRequest::operator=(const HttpRequest& rhs) {
	(void)rhs;
	return *this;
}

