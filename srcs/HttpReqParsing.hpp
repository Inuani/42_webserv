#include <iostream>
#include <map>

#ifndef HTTPREQPARSING_HPP
# define HTTPREQPARSING_HPP

class HttpReqParsing {

public:

	HttpReqParsing(const std::string& strHttpRequest);
	~HttpReqParsing();

	const std::string&	getMethod() const;
	const std::string&	getUri() const;
	const std::string&	getVersion() const;
	const std::map<std::string, std::string>&	getheaders() const;
	const std::string&	getBody() const;

private:

	HttpReqParsing();
	HttpReqParsing(const HttpReqParsing& src);
	HttpReqParsing& operator=(const HttpReqParsing& rhs);

	std::string							_method;
	std::string							_uri;
	std::string							_version;
	std::map<std::string, std::string>	_headers;
	std::string							_body;

};

#endif
