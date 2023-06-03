#include "Server.hpp"
#include "utils.hpp"
#include "Settings.h"

int main(int argc, char *argv[])
{
	if (argc > 2)
	{
		std::cerr << "too many arguments" << std::endl;
		exit(1);
	}
	std::vector<Settings> setts;
	if (argc == 1)
		getConfig(setts, "");
	if (argc == 2)
		getConfig(setts, argv[1]);

	Serv s;

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
