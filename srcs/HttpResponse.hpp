#include <iostream>

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

class HttpResponse {

public:

	HttpResponse(int status, const std::string& body, const std::string& contentType);
	~HttpResponse();

	const std::string&	toString() const;
	const std::string&	getStatusMessage(int status) const;

private:

	int			_status;
	std::string	_body;
	std::string	_contentType;

	HttpResponse();
	HttpResponse(const HttpResponse& src);
	HttpResponse& operator=(const HttpResponse& rhs);

};

#endif
