#include "ErrorHandler.hpp"

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
    if (!path.empty())
    {
        readFile(path);
    }
    else
    {
        _body = _strStatusCode;
    }  
}
