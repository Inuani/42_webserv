#include <fstream>
#include <sstream>
#include "utils.hpp"
#include <iostream>

std::string readFileContent(const std::string& path) {
	std::ifstream file(path, std::ios::binary);
	std::cout << path;
	std::ifstream error404("www/html/404.html", std::ios::binary);
	std::ostringstream ss;
	if (!file.is_open()) 
	{
		std::cerr << "Failed to open file: " << path << '\n';
		ss << error404.rdbuf();
		std::cout << "lol\n";
	}
	else
		ss << file.rdbuf();
	std::cout << "Returning : " << ss.str() << "\n";
	return ss.str();
}

std::string getFileType(const std::string& filePath)
{
	std::string ext;
	std::string fileType;
	ext.insert(0, filePath, filePath.find_last_of("."), std::string::npos);

	if (ext == ".html")
		fileType = "text/html";
	else if (ext == ".jpg")
		fileType = "image/jpeg";
	else if (ext == ".gif")
		fileType = "image/gif";
	//std::cout << "\next is : " << ext << "\n";
	//std::cout << "\nfile Type is : " << filePath << "\n";
	return (fileType);
}