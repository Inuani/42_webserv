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
	for (std::multimap<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it) {
		response += it->first + ": " + it->second + "\r\n";
	}
	response += "\r\n";
	response += _body;
	return response;
}

const char*	HttpResponse::getStatusMessage(int status) const {
	switch(status) {
		case 100: return "Continue";
		case 101: return "Switching Protocols";
		case 200: return "OK";
		case 201: return "Created";
		case 202: return "Accepted";
		case 203: return "Non-Authoritative Information";
		case 204: return "No Content";
		case 205: return "Reset Content";
		case 206: return "Partial Content";
		case 300: return "Multiple Choices";
		case 301: return "Moved Permanently";
		case 302: return "Found";
		case 303: return "See Other";
		case 304: return "Not Modified";
		case 305: return "Use Proxy";
		case 307: return "Temporary Redirect";
		case 400: return "Bad Request";
		case 401: return "Unauthorized";
		case 402: return "Payment Required";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 406: return "Not Acceptable";
		case 407: return "Proxy Authentication Required";
		case 408: return "Request Timeout";
		case 409: return "Conflict";
		case 410: return "Gone";
		case 411: return "Length Required";
		case 412: return "Precondition Failed";
		case 413: return "Payload Too Large";
		case 414: return "URI Too Long";
		case 415: return "Unsupported Media Type";
		case 416: return "Range Not Satisfiable";
		case 417: return "Expectation Failed";
		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		case 502: return "Bad Gateway";
		case 503: return "Service Unavailable";
		case 504: return "Gateway Timeout";
		case 505: return "HTTP Version Not Supported";
		default: return "Unknown Status";
	}
}

void	HttpResponse::setHeaders(const std::string& key, const std::string& value){
	_headers.insert(std::make_pair(key, value));
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
