#include <iostream>

#ifndef REQHANDLER_HPP
# define REQHANDLER_HPP

class ReqHandler {

public:

	ReqHandler();
	ReqHandler(const ReqHandler& src);
	~ReqHandler();

	ReqHandler& operator=(const ReqHandler& rhs);

private:

};

#endif
