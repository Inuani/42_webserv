#include <iostream>
#include <map>


#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

class HttpResponse {

public:


	HttpResponse(int status, const std::string& body);
	~HttpResponse();
	std::string			toString() const;
	const char*			getStatusMessage(int status) const;

	void				setHeaders(const std::string& key, const std::string& value);

private:

	int									_status;
	std::string							_body;
	std::multimap<std::string, std::string> _headers;

	HttpResponse();
	HttpResponse(const HttpResponse& src);
	HttpResponse& operator=(const HttpResponse& rhs);

};

#endif
