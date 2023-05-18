#include "HttpResponse.hpp"
#include <iostream>

HttpResponse::HttpResponse(int status, const std::string& body, const std::string& contentType) 
	: _status(status), _body(body), _contentType(contentType) {
}

HttpResponse::~HttpResponse() {}

std::string	HttpResponse::toString() const {
	std::string	response;
	response += "HTTP/1.1 " + std::to_string(_status) + " " + getStatusMessage(_status) + "\r\n";
	response += "Content-Type : " + _contentType + "\r\n";
	response += "Content-Length : " + std::to_string(_body.size()) + "\r\n";
	if (!_location.empty())
		response += "Location : " + _location + "\r\n";
	response += "\r\n";
	response += _body;
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

void HttpResponse::setLocationHeader(const std::string& location) {
	_location = location;
}


HttpResponse::HttpResponse() {}

HttpResponse::HttpResponse(const HttpResponse &src) {
	*this = src;
}

HttpResponse&	HttpResponse::operator=(const HttpResponse& rhs) {
	(void)rhs;
	return *this;
}
