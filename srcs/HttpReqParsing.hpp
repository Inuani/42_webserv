#include <iostream>
#include <map>

#ifndef HTTPREQPARSING_HPP
# define HTTPREQPARSING_HPP

class HttpReqParsing {

public:

	HttpReqParsing(const std::string& strHeader, const std::string& strBody);
	HttpReqParsing(const std::string& strHttpRequest);
	~HttpReqParsing();

	const std::string&							getMethod() const;
	const std::string&							getUri() const;
	const std::string&							getVersion() const;
	const std::map<std::string, std::string>&	getHeaders() const;
	const std::string&							getBody() const;
	const std::string&							getQueryString() const;
	const std::string&							getHeadersValue(const std::string& key) const;

private:

	HttpReqParsing();
	HttpReqParsing(const HttpReqParsing& src);
	HttpReqParsing& operator=(const HttpReqParsing& rhs);

	void	_parseHeader(const std::string& strHeader);
	void	_queryStr2Map(const std::string& queryString);

	std::string							_method;
	std::string							_uri;
	std::string							_version;
	std::map<std::string, std::string>	_headers;
	std::string							_body;
	std::string							_queryString;
	std::map<std::string, std::string>	_queryMap;

};

#endif
