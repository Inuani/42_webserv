
#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <vector>

struct Location
{
	std::string path;
	std::string root;
	std::string index;
	std::string methods;
	std::string err_pages;
};

struct Settings
{
	int port;
	std::size_t max_body;
	std::string	root;
	std::string	index;
	std::string err_pages;
	std::string server_name;
	std::vector<Location> location;
};

void locs_debug(Location location);
void setts_debug(Settings settings);
void getConfig(std::vector<Settings> &setts);

#endif

