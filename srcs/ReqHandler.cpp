#include "ReqHandler.hpp"
#include <iostream>

ReqHandler::ReqHandler()
{
}

ReqHandler::ReqHandler(const ReqHandler &src)
{
	*this = src;
}

ReqHandler::~ReqHandler()
{
}

ReqHandler&	ReqHandler::operator=(const ReqHandler& rhs)
{
	(void)rhs;
	return *this;
}
