
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
	std::string error;
	std::string dir_listing;
	std::string ext;
};

struct Settings
{
	int port;
	std::size_t	max_body;
	std::string	root;
	std::string	index;
	std::string	error;
	std::string	server_name;
	std::string dir_listing;
	std::vector<Location> location;
	std::map<std::string, std::string> redirect;
};

void locs_debug(Location location);
void setts_debug(Settings settings);
void getConfig(std::vector<Settings> &setts);

#endif
