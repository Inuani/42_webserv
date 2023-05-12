#include <iostream>
#include <map>

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

class HttpRequest {

public:

	HttpRequest(const std::string& strHttpRequest);
	~HttpRequest();

	const std::string&	getMethod() const;
	const std::string&	getUri() const;
	const std::string&	getVersion() const;
	const std::map<std::string, std::string>&	getheaders() const;
	const std::string&	getBody() const;

private:

	HttpRequest();
	HttpRequest(const HttpRequest& src);
	HttpRequest& operator=(const HttpRequest& rhs);

	std::string							_method;
	std::string							_uri;
	std::string							_version;
	std::map<std::string, std::string>	_headers;
	std::string							_body;

};

#endif
