
#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <vector>
#include <map>

struct Location
{
	std::string path;
	std::string root;
	std::string index;
	std::string methods;
	std::string dir_listing;
	std::string ext;
};

struct Settings
{
	int port;
	std::size_t	max_body;
	std::string ext;
	std::string methods;
	std::string	root;
	std::string	index;
	std::string	server_name;
	std::string dir_listing;
	std::vector<Location> location;
	std::map<std::string, std::string> redirect;
	std::map<std::string, std::string> error_pages;
};

void locs_debug(Location location);
void setts_debug(Settings settings);
void getConfig(std::vector<Settings> &setts, std::string);

#endif
