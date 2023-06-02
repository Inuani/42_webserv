#include <fstream>
#include <sstream>
#include "utils.hpp"
#include <iostream>
#include "ContentTypes.hpp"
#include "dirent.h"
#include <stdio.h>
#include <sstream>

std::string readFileContent(const std::string& path) {
	std::ifstream file(path, std::ios::binary);
	std::ifstream error404("www/html/404.html", std::ios::binary);
	std::ostringstream ss;
	if (!file.is_open()) 
	{
		std::cerr << "Failed to open file: " << path << '\n';
		//ss << error404.rdbuf();
		throw 404;
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
	
	int blablabla;
	len = header.substr(header.find("Content-Length:") + 16, (header.find("\n", header.find("Content-Length:"))));
	std::istringstream oss(len);
	oss >> blablabla;
	return (blablabla);
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

const Location* findLocationByRoot(const Settings& set, const std::string& root)
{
	std::vector<Location>::const_iterator it;
	for (it = set.location.begin(); it != set.location.end(); it++)
	{
		if (it->root == root)
			return &(*it);
	}
	return (NULL);
}

std::string getURLStart(const Settings& set, std::string directoryPath)
{
	std::string extraPath;
	std::string locationRoot = directoryPath;
	const Location* location;
	while (locationRoot.find("/") != std::string::npos)
	{
		location = findLocationByRoot(set, locationRoot);
		if (location)
		{
			break;
		}
		extraPath = locationRoot.substr(locationRoot.find_last_of('/'), std::string::npos) + extraPath;
		locationRoot = locationRoot.substr(0, locationRoot.find_last_of('/'));
	}
	if (location == NULL)
	{
		// if there is a root directory in the settings, and it is at the start of the path, we replace it with a simple /
		if (!set.root.empty() && directoryPath.find(set.root.c_str(), 0, set.root.length() - 1))
		{
			extraPath = directoryPath.substr(set.root.length() - 1, std::string::npos);
			locationRoot = "/";
		}
	}
	else
		locationRoot = location->path;
	return (locationRoot + extraPath);
}

const std::string	repertoryListing(const Settings& set, std::string directoryPath)
{
	std::stringstream	htmlFile;
	DIR* 				dir;
	struct dirent*		entry;

	// std::cout << "dirPath is : " << directoryPath << std::endl;
	std::string urlStart = getURLStart(set, directoryPath);
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
			// std::cout << "link is " << link << std::endl;
			if (entry->d_type == DT_DIR) {
				link += "/";
			}
			htmlFile << "<a href=\"" << urlStart + link << "\">" << name << "</a><br>";
		}
		htmlFile << "</body></html>";
		closedir(dir);
		// std::cout << "Directory listing generated successfully." << std::endl;
	}
	return (htmlFile.str());
}
