#include "HttpReqParsing.hpp"
#include <iostream>
#include <sstream>
#include <map>

HttpReqParsing::HttpReqParsing(const std::string& strHttpRequest) {
	_parseHeader(strHttpRequest);
}

HttpReqParsing::HttpReqParsing(const std::string& strHeader, const std::string& strBody, const Settings& settings) 
	: _body(strBody), _settings(settings)
{
	_parseHeader(strHeader);
	// std::cout << std::endl;
	// std::cout << "<--------------- start body --------------->" << std::endl;
	// std::cout << _body << std::endl;
	// std::cout << "<--------------- end request --------------->" << std::endl;
}


void	HttpReqParsing::_splitUriWithLocations() {
	// std::cout << "----------------- DEBUG START -----------------" << std::endl;
	//std::cout << "Uri is " << _uri << std::endl;
	for (std::vector<Location>::const_iterator it = _settings.location.begin(); it != _settings.location.end(); ++it)
	{
		if (_uri.find(it->path) == 0 && !(_uri.compare(0, it->path.length(), it->path)) && it->path.length() > _locationPath.length())
			_locationPath = it->path;
	}
	// std::cout << "--- locationPath found is " << _locationPath << std::endl;
	if (_uri.find(_locationPath) == 0)
	{
		_uri = _uri.substr(_locationPath.length());
		_reqLocation = findLocationByPath(_settings, _locationPath);
		_filename = _uri;
		if (_reqLocation)
			_filedir = _reqLocation->root;
		else
			_filedir = _settings.root;
		if (_filename.find("/") != 0)
			_filename = "/" + _filename;
		//std::cout << "Full path : " << _filedir << " + " << _filename << std::endl;
	}
	// std::cout << "------------------ DEBUG END ------------------" << std::endl;
}

void HttpReqParsing::_parseHeader(const std::string& strHeader)
{
	std::cout << "??" << std::endl;
	if (getContentLen(strHeader) > _settings.max_body)
		throw 413;
	std::cout << (getContentLen(strHeader) << _settings.max_body) << std::endl;
	std::istringstream parseStream(strHeader);
	std::string line;

	std::getline(parseStream, line);
	std::istringstream requestStream(line);

	requestStream >> _method >> _uri >> _version;
	if (_uri.length() > 8192)
		throw 414;
	
	// std::cout << "<--------------- new start --------------->" << std::endl;
	// std::cout << "start uri is : " << _uri << std::endl;
	size_t pos = _uri.find('?');
	if (pos != std::string::npos) {
		_queryString = _uri.substr(pos + 1);
		_queryStr2Map(_queryString);
		_uri = _uri.substr(0, pos);
	}

	pos = _uri.find('.');
	if (pos != std::string::npos) {
		_fileExt = _uri.substr(pos + 1);
		// std::cout << _fileExt << std::endl;
	}

	// std::cout << "before pathinfo uri : "<< _uri << std::endl;

	pos = _fileExt.find('/');
	if (pos != std::string::npos) {
		_pathInfo = _fileExt.substr(pos, std::string::npos);
		// std::cout << "pathinfo is " << _pathInfo << std::endl;
		_fileExt = _fileExt.substr(0, pos);
		// std::cout << "file ext is : " << _fileExt << std::endl;
		pos = _uri.find_last_of(_pathInfo);
		_uri = _uri.substr(0, pos - (_pathInfo.length() - 1));
	}



	// std::cout << "initial uri : "<< _uri << std::endl;
	_splitUriWithLocations();
	// std::cout << "transformed uri : "<< _uri << std::endl;
	//std::cout << "location path : "<< _locationPath << std::endl;

	// std::cout << "<--------------- new end --------------->" << std::endl;

	

	// Location *location = findLocationByPath(_settings, );
	// if (location != NULL && location->methods.find() == std::string::npos) {
	// 	// 405 Method Not Allowed
	// 	throw 405;
	// }

	// std::cout << "<--------------- start request --------------->" << std::endl;
	// std::cout << _method << " " << _uri << " " << _version << std::endl;	


	while (std::getline(parseStream, line) && line != "\r") {
		std::istringstream headerStream(line);
		std::string key, value;
		std::getline(headerStream, key, ':');
		key.erase(key.find_last_not_of(" \n\r\t") + 1);
		std::getline(headerStream, value);
		size_t start = value.find_first_not_of(" \n\r\t");
        if (start != std::string::npos) {
            value = value.substr(start);
        }
		value.erase(value.find_last_not_of(" \n\r\t") + 1);
		_headers[key] = value;
		// std::cout << key << " : " << value << std::endl;
	}
}

HttpReqParsing::~HttpReqParsing() {}

const std::string	HttpReqParsing::getHeadersValue(const std::string& key) const {
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it != _headers.end()) {
		return it->second;
	}
	return "";
}

const std::string	HttpReqParsing::getQueryValue(const std::string& key) const {
	std::map<std::string, std::string>::const_iterator it = _queryMap.find(key);
	if (it != _queryMap.end()) {
		return it->second;
	}
	return "";
}

std::string	HttpReqParsing::_decodeUrl(const std::string& qS) {
	std::string	decoded;
	for (size_t i = 0; i < qS.length(); ++i) {
		if (qS[i] == '+') {
			decoded += ' ';
		} else if (qS[i] == '%' && i + 2 < qS.length()) {
			std::string hexStr = qS.substr(i + 1, 2);
			std::istringstream hexS(hexStr);
			int hex2int;
			hexS >> std::hex >> hex2int;
			char c = static_cast<char>(hex2int);
			decoded += c;
			i += 2;
		} else {
			decoded += qS[i];
		}
	}
	return decoded;
}

void	HttpReqParsing::_queryStr2Map(const std::string& queryString) {
	std::istringstream	queryStream(queryString);
	std::string			token;
	while (std::getline(queryStream, token, '&')) {
		size_t pos = token.find('=');
		if (pos != std::string::npos) {
			std::string key = token.substr(0, pos);
			std::string value = token.substr(pos + 1);
			key = _decodeUrl(key);
			value = _decodeUrl(value);
			_queryMap[key] = value;
		}
	}
}

const std::string&	HttpReqParsing::getMethod() const {
	return _method;
}

const std::string&	HttpReqParsing::getUri() const {
	return _uri;
}

const std::string&	HttpReqParsing::getVersion() const {
	return _version;
}

const std::map<std::string, std::string>&	HttpReqParsing::getHeaders() const {
	return _headers;
}

const std::string&	HttpReqParsing::getBody() const {
	return _body;
}

const std::string&	HttpReqParsing::getQueryString() const {
	return _queryString;
}

const std::string&	HttpReqParsing::getPathInfo() const {
	return _pathInfo;
}

const std::string&	HttpReqParsing::getfileExt() const {
	return _fileExt;
}

std::string	HttpReqParsing::getFileName() const
{
	return(_filename);
}

std::string	HttpReqParsing::getFileDir() const
{
	return (_filedir);
}
const Location*	HttpReqParsing::getFileLocation() const
{
	return (_reqLocation);
}

HttpReqParsing::HttpReqParsing() {}

HttpReqParsing::HttpReqParsing(const HttpReqParsing &src) {
	*this = src;
}

HttpReqParsing&	HttpReqParsing::operator=(const HttpReqParsing& rhs) {
	(void)rhs;
	return *this;
}
