#include "DefaultHandler.hpp"
#include <iostream>

DefaultHandler::DefaultHandler()
{
}

DefaultHandler::DefaultHandler(const DefaultHandler &src)
{
	*this = src;
}

DefaultHandler::~DefaultHandler()
{
}

DefaultHandler&	DefaultHandler::operator=(const DefaultHandler& rhs)
{
	(void)rhs;
	return *this;
}
