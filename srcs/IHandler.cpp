#include "IHandler.hpp"
#include <iostream>

IHandler::IHandler()
{
}

IHandler::IHandler(const IHandler &src)
{
	*this = src;
}

IHandler::~IHandler()
{
}

IHandler&	IHandler::operator=(const IHandler& rhs)
{
	(void)rhs;
	return *this;
}
