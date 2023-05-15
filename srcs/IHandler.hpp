#include <iostream>

#ifndef IHANDLER_HPP
# define IHANDLER_HPP

class IHandler {

public:

	IHandler();
	virtual ~IHandler();

	virtual void handle(const HttpReqParsing& request, ) = 0;

private:

};

#endif
