#include "Server.hpp"
#include "utils.hpp"
#include "Settings.h"

int main(int argc, char *argv[])
{
	std::vector<Settings> setts;
	getConfig(setts);

	Serv s;
	for(std::vector<Settings>::iterator it = setts.begin(); it != setts.end(); it++)
	{
		setts_debug(*it);
		std::vector<Location> locs = it->location;
		for(std::vector<Location>::iterator it2 = locs.begin(); it2 != locs.end(); it2++)
			locs_debug(*it2);
	}

	if (setts.empty())
	{
		std::cerr << "set a config" << std::endl;
		exit(1);
	}
	s.settings_setter(setts);
	s.setBindAddrinfo();
	s.srvListen();
	s.setEvent();

}
