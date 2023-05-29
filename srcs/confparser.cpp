
#include <iostream>
#include <fstream>
#include <sstream>

#include <map>
#include <vector>

#include "Settings.h"
#include "Server.hpp"


void setts_debug(struct Settings settings)
{
	// std::cout << settings.port << std::endl;
	// std::cout << settings.max_body << std::endl;
	// std::cout << settings.err_pages << std::endl;
	// std::cout << settings.server_name << std::endl;
	// std::cout << settings.root << std::endl;
	// std::cout << settings.index << std::endl;
	// std::cout << settings. << std::endl;
}

void locs_debug(struct Location location)
{
	// std::cout << location.path << std::endl;
	// std::cout << location.root << std::endl;
	// std::cout << location.index << std::endl;
	// std::cout << location.methods << std::endl;
	// std::cout << location.err_pages << std::endl;
}

//max_body must be 0
void default_sett(struct Settings &settings)
{
	settings.port = 8080;
	settings.max_body = 0;
	settings.err_pages = "";
}

void default_loc(struct Location &location)
{
	location.path = "/";
	location.root = "www/html";
	location.index = "index.html";
	location.methods = "GET,POST,DELETE,PUT";
	location.err_pages = "";
}

Location create_location(std::stringstream &file, std::string path)
{
	Location location;
	location.path = path;
	std::string word;
	std::string line;

	while (std::getline(file, line))
	{
		std::istringstream ssline(line);
		while(ssline >> line)
		{
			word = "";
			if(line == "root") {
				ssline >> word;
				location.root = word;
			}
			else if(line == "index") {
				ssline >> word;
				location.index = word;
			}
			else if(line == "methods") {
				while(ssline >> line)
					word += line;
				location.methods = word;
			}
			else if(line == "}")
				return location;
			line = "";
		}
	}
	return location;
}

Settings create_settings(std::stringstream &file)
{
	Settings settings;
	size_t number;
	std::string line;
	std::string word;

	default_sett(settings);
	while (std::getline(file, line))
	{
		std::istringstream ssline(line);

		while(ssline >> line)
		{
			word = "";
			if (line == "root") {
				ssline >> word;
				settings.root = word;
			}
			else if (line == "index") {
				ssline >> word;
				settings.index = word;
			}
			if (line == "listen") {
				ssline >> number;
				if ((number <= 65535) && (number >= 0))
					settings.port = number;
				else {
					std::cerr << "Invalid port number" << std::endl;
					exit(1);
				}
			}
			if (line == "max_body") {
				ssline >> number;
				if (number > (size_t)1000000000000000000)
				{
					std::cerr << "MAX_BODY TOO BIG" << std::endl;
					exit(1);
				}
				settings.max_body = number;
			}
			else if(line == "server_name")
			{
				while(ssline >> line)
					word += line;
				ssline >> line;
				settings.server_name = word;
			}
			else if(line == "location")
			{
				ssline >> line;
				//create_location(file, line);
				settings.location.push_back(create_location(file, line));
			}
			else if(line == "}")
				return settings;
			line = "";
		}
	}
	return settings;
}


void getConfig(std::vector<Settings> &setts)
{
	std::string line;
	std::ifstream conf("webserv.conf");
	if (conf.fail())
		std::cerr << "Config file: Error: " << strerror(errno) << std::endl;

	std::stringstream file;
	file << conf.rdbuf();
	while (std::getline(file, line))
	{
		if (line.find("server") != std::string::npos)//&& line == "server {")
			setts.push_back(create_settings(file));
	}
}
