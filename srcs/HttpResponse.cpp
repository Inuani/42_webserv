#include "HttpResponse.hpp"
#include <iostream>
#include <sstream>

// HttpResponse::HttpResponse(int status, const std::string& body, const std::string& contentType) 
// 	: _status(status), _body(body), _contentType(contentType) {
// }

HttpResponse::HttpResponse(int status, const std::string& body) 
	: _status(status), _body(body) {
}

HttpResponse::~HttpResponse() {}

std::string	HttpResponse::toString() const {
	std::string	response;
	std::ostringstream oInt;
	oInt << _status;
	std::string	int2Str = oInt.str();
	response += "HTTP/1.1 " + int2Str + " " + getStatusMessage(_status) + "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it) {
		response += it->first + ": " + it->second + "\r\n";
	}
	response += "\r\n";
	response += _body;
	// std::cout << response << std::endl;
	return response;
}

const char*	HttpResponse::getStatusMessage(int status) const {
	switch(status) {
		case 200: return "OK";
		case 303: return "See Other";
		case 404: return "Not Found";
		default: return "Unknown Status";
	}
}

void	HttpResponse::setHeaders(const std::string& key, const std::string& value){
	_headers[key] = value;
}

// void HttpResponse::setLocationHeader(const std::string& location) {
// 	_location = location;
// }


HttpResponse::HttpResponse() {}

HttpResponse::HttpResponse(const HttpResponse &src) {
	*this = src;
}

HttpResponse&	HttpResponse::operator=(const HttpResponse& rhs) {
	(void)rhs;
	return *this;
}
