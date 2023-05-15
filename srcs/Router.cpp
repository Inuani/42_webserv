#include "Router.hpp"
#include <iostream>

Router::Router()
{
}

Router::Router(const Router &src)
{
	*this = src;
}

Router::~Router()
{
}

Router&	Router::operator=(const Router& rhs)
{
	(void)rhs;
	return *this;
}
