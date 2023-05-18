#include <fstream>
#include <sstream>
#include "utils.hpp"
#include <iostream>

std::string readFileContent(const std::string& path) {
	std::ifstream file(path, std::ios::binary);
	std::ifstream error404("www/html/404.html", std::ios::binary);
	std::ostringstream ss;
	if (!file.is_open()) 
	{
		std::cerr << "Failed to open file: " << path << '\n';
		ss << error404.rdbuf();
	}
	else
		ss << file.rdbuf();
	return ss.str();
}

std::string getFileType(const std::string& filePath)
{
	std::string ext;
	std::string fileType;
	if (filePath.find_last_of(".") == std::string::npos)
		return ("text/html");

	ext.insert(0, filePath, filePath.find_last_of("."), std::string::npos);
	
	if (ext == ".html")
		fileType = "text/html";
	else if (ext == ".jpg")
		fileType = "image/jpeg";
	else if (ext == ".gif")
		fileType = "image/gif";

	return (fileType);
}

int getResponseCode(const std::string& filePath)
{
	std::ifstream file(filePath, std::ios::binary);
	if (!file.is_open())
	{
		return 404;
	}
	return 200;
}

int getContentLen(const std::string& header)
{
	std::string len;
	len = header.substr(header.find("Content-Length:") + 16, (header.find("\n", header.find("Content-Length:"))));
	return (std::stoi(len));
}