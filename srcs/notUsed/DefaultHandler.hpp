#include <iostream>

#ifndef DEFAULTHANDLER_HPP
# define DEFAULTHANDLER_HPP

class DefaultHandler {

public:

	DefaultHandler();
	DefaultHandler(const DefaultHandler& src);
	~DefaultHandler();

	DefaultHandler& operator=(const DefaultHandler& rhs);

private:

};

#endif
