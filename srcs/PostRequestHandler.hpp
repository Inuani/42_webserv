#include <iostream>

#ifndef POSTREQUESTHANDLER_HPP
# define POSTREQUESTHANDLER_HPP

class PostRequestHandler {

public:

	PostRequestHandler();
	PostRequestHandler(const PostRequestHandler& src);
	~PostRequestHandler();

	PostRequestHandler& operator=(const PostRequestHandler& rhs);

private:

};

#endif
