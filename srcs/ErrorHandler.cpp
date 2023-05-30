#include "ErrorHandler.hpp"
#include <iostream>

ErrorHandler::ErrorHandler(const int statusCode, const std::map<std::string, std::string> errorFiles, const Settings& settings) : _intStatusCode(statusCode), _errorFileMap(errorFiles), _settings(settings)
{
	std::ostringstream oss;
	oss << statusCode;
	_strStatusCode = oss.str();
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

std::string ErrorHandler::getErrorFile() const
{
	std::map<std::string, std::string>::const_iterator it = _errorFileMap.find(_strStatusCode);
	if (it != _errorFileMap.end())
	{
		if (access(it->second.c_str(), R_OK) == 0)
			return (it->second);
	}
	std::string defaultPath = "/www/error/" + _strStatusCode + ".html";
	if (access(defaultPath.c_str(), R_OK) == 0)
		return (defaultPath);
	return ("");
}

void ErrorHandler::generateBody()
{
	std::string path = getErrorFile();
	//path = "/404.html";
	if (!path.empty())
	{
		findFileLocation(path);
		readFile(_filedir + _filename);
	}
	else
	{
		_body = _strStatusCode;
	}
}

void ErrorHandler::findFileLocation(const std::string& filePath)
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
		// if settings.index is not default value
		_filedir = _settings.root;
	}
	else
		_filedir = location->root;
}