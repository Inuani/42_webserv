#include "PostRequestHandler.hpp"
#include <iostream>

PostRequestHandler::PostRequestHandler()
{
}

PostRequestHandler::PostRequestHandler(const PostRequestHandler &src)
{
	*this = src;
}

PostRequestHandler::~PostRequestHandler()
{
}

PostRequestHandler&	PostRequestHandler::operator=(const PostRequestHandler& rhs)
{
	(void)rhs;
	return *this;
}
