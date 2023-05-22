#include "HttpReqParsing.hpp"
#include <iostream>
#include <sstream>
#include <map>

HttpReqParsing::HttpReqParsing(const std::string& strHttpRequest) {
	parseHeader(strHttpRequest);
}

HttpReqParsing::HttpReqParsing(const std::string& strHeader, const std::string& strBody) : _body(strBody)
{
	parseHeader(strHeader);
	std::cout << std::endl;
	std::cout << _body << std::endl;
	std::cout << "<--------------- end request --------------->" << std::endl;
}

void HttpReqParsing::parseHeader(const std::string& strHeader)
{
	// std::cout << "<--------------- raw request --------------->" << std::endl;
	// std::cout << strHeader << std::endl;
	// std::cout << "<--------------- end raw request --------------->" << std::endl;
	// std::cout << std::endl;

	std::istringstream parseStream(strHeader);
	std::string line;

	std::getline(parseStream, line);
	std::istringstream requestStream(line);
	requestStream >> _method >> _uri >> _version;

	size_t pos = _uri.find('?');
	if (pos != std::string::npos) {
		_queryString = _uri.substr(pos + 1);
		_uri = _uri.substr(0, pos);
	}

	std::cout << "<--------------- start request --------------->" << std::endl;
	std::cout << _method << " " << _uri << " " << _version << std::endl;	
	if (!_queryString.empty()) {
		std::cout << "query string : " << _queryString << std::endl;	
	}

	while (std::getline(parseStream, line) && line != "\r") {
		std::istringstream headerStream(line);
		std::string key, value;
		std::getline(headerStream, key, ':');
		// headerStream >> value;
		headerStream.ignore(1); //caution
		std::getline(headerStream, value);
		_headers[key] = value;
		std::cout << key << " : " << value <<std::endl;
	}
}

HttpReqParsing::~HttpReqParsing() {}

const std::string&	HttpReqParsing::getHeadersValue(const std::string& key) const {
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it != _headers.end()) {
		return it->second;
	}
	return "";
}

const std::string&	HttpReqParsing::getMethod() const {
	return _method;
}

const std::string&	HttpReqParsing::getUri() const {
	return _uri;
}

const std::string&	HttpReqParsing::getVersion() const {
	return _version;
}

const std::map<std::string, std::string>&	HttpReqParsing::getHeaders() const {
	return _headers;
}

const std::string&	HttpReqParsing::getBody() const {
	return _body;
}

const std::string&	HttpReqParsing::getQueryString() const {
	return _queryString;
}

HttpReqParsing::HttpReqParsing() {}

HttpReqParsing::HttpReqParsing(const HttpReqParsing &src) {
	*this = src;
}

HttpReqParsing&	HttpReqParsing::operator=(const HttpReqParsing& rhs) {
	(void)rhs;
	return *this;
}

