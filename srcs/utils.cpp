#include <fstream>
#include <sstream>
#include "utils.hpp"
#include <iostream>
#include "ContentTypes.hpp"
#include "dirent.h"
#include <stdio.h>

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

	std::map<std::string, std::string>::iterator it = mimeTypes.find(ext);
	
	if (it != mimeTypes.end()) {
		return it->second;
	}
	return "application/octet-stream";
	// if (ext == ".html")
	// 	fileType = "text/html";
	// else if (ext == ".jpg")
	// 	fileType = "image/jpeg";
	// else if (ext == ".gif")
	// 	fileType = "image/gif";
	// else if (ext == ".ico")
	// 	fileType = "image/x-icon";

	// return (fileType);
}

int getResponseCode(const std::string& filePath)
{
	std::ifstream file(filePath, std::ios::binary);
	if (!file.is_open())
	{
		throw 404;
	}
	return 200;
}

size_t getContentLen(const std::string& header)
{
	std::string len;
	len = header.substr(header.find("Content-Length:") + 16, (header.find("\n", header.find("Content-Length:"))));
	return (std::stoi(len));
}

const Location* findLocationByPath(const Settings& set, const std::string& path) {
	std::vector<Location>::const_iterator it;
	for (it = set.location.begin(); it != set.location.end(); ++it) {
		if (it->path == path) {
			return &(*it);
		}
	}
	return NULL;
}

const std::string	repertoryListing(const std::string directoryPath, const Location* location)
{
	std::stringstream	htmlFile;
	DIR* 				dir;
	struct dirent*		entry;

	std::cout << "dirPath is : " << directoryPath << std::endl;
	if ((dir = opendir(directoryPath.c_str())) != NULL)
	{
		htmlFile << "<!DOCTYPE html>";
		htmlFile << "<html><head><title>Directory Listing</title>";
		htmlFile << "<style>";
		htmlFile << "body {font-family: Arial, sans-serif; padding: 20px;}";
		htmlFile << "h1 {color: #5a5a5a;}";
		htmlFile << ".file-link {display: block; margin: 5px 0;}";
		htmlFile << ".directory {color: #1a73e8;}";
		htmlFile << "</style>";
		htmlFile << "</head><body>";
		htmlFile << "<h1>Directory Listing</h1>";

		// Iterate over the files and directories
		while ((entry = readdir(dir)) != NULL)
		{
			std::string name = entry->d_name;
			// Ignore hidden files and special directories (. and ..)
			if (name[0] == '.') {
				continue;
			}
			std::string link = name;
			if (location)
				link = location->path + "/" + name;
			if (entry->d_type == DT_DIR) {
				link += "/";
			}
			htmlFile << "<a href=\"" << link << "\">" << name << "</a><br>";
		}
		htmlFile << "</body></html>";
		closedir(dir);
		std::cout << "Directory listing generated successfully." << std::endl;
	}
	return (htmlFile.str());
}
