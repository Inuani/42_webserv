#include "ErrorHandler.hpp"
#include <iostream>

ErrorHandler::ErrorHandler(const int statusCode, const std::map<std::string, std::string> errorFiles, const Settings& settings) : _intStatusCode(statusCode), _errorFileMap(errorFiles), _settings(settings)
{
	std::ostringstream oss;
	oss << statusCode;
	_strStatusCode = oss.str();
	_hasSettings = 1;
}

ErrorHandler::ErrorHandler(const int statusCode) : _intStatusCode(statusCode)
{
	std::ostringstream oss;
	oss << statusCode;
	_strStatusCode = oss.str();
	_hasSettings = 0;
}

ErrorHandler::~ErrorHandler(){}

std::string ErrorHandler::getBody() const
{
	return (_body);
}

void ErrorHandler::readFile(const std::string& path)
{
	std::ifstream file;
	std::ostringstream ss;

	file.open(path);
	if (file.is_open()) 
		ss << file.rdbuf();
	_body = ss.str();
}

std::string ErrorHandler::getErrorFile()
{
	if (_hasSettings == 0)
	{
		std::string defaultPath = "./www/error/" + _strStatusCode + ".html";
		if (access(defaultPath.c_str(), R_OK) == 0)
			return (defaultPath);
		return ("");
	}
	std::map<std::string, std::string>::const_iterator it = _errorFileMap.find(_strStatusCode);
	if (it != _errorFileMap.end())
	{
		std::string path = findFileLocation(it->second);
		if (access(path.c_str(), R_OK) == 0)
			return (path);
	}
	std::string defaultPath = "./www/error/" + _strStatusCode + ".html";
	if (access(defaultPath.c_str(), R_OK) == 0)
		return (defaultPath);
	return ("");
}

void ErrorHandler::generateBody()
{
	std::string path = getErrorFile();
	if (!path.empty())
	{
		readFile(path);
	}
	else
	{
		_body = _strStatusCode;
	}
}

std::string ErrorHandler::findFileLocation(std::string filePath)
{
	_filename = filePath.substr(filePath.find_last_of('/'), std::string::npos);
	_filedir = filePath.substr(0, filePath.find_last_of('/'));
	const Location* location;
	while (!_filedir.empty())
	{
		location = findLocationByPath(_settings, _filedir);
		if (location)
		{
			break;
		}
		_filename = _filedir.substr(_filedir.find_last_of('/'), std::string::npos) + _filename;
		_filedir = _filedir.substr(0, _filedir.find_last_of('/'));
	}
	if (_filedir.empty())
		location = findLocationByPath(_settings, "/");
	if (location == NULL)
	{
		if (!_settings.root.empty())
			_filedir = _settings.root;
	}
	else
		_filedir = location->root;
	return (_filedir + _filename);
}
