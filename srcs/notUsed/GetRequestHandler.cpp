#include "GetRequestHandler.hpp"
#include <iostream>

GetRequestHandler::GetRequestHandler() {}

GetRequestHandler::~GetRequestHandler() {}

GetRequestHandler::GetRequestHandler(const GetRequestHandler &src) {
	*this = src;
}

GetRequestHandler&	GetRequestHandler::operator=(const GetRequestHandler& rhs) {
	(void)rhs;
	return *this;
}
