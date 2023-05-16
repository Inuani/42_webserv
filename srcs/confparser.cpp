
#include "Server.hpp"

#include <map>

//temp vars;

std::string location;
std::string root;
std::string indexx;
std::string methods;
std::string b;
std::string c;


void parse_location(std::stringstream &file)
{
	std::string word;
	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream ssline(line);
		while(ssline >> line)
		{
			if(line == "root") {
				ssline >> word;
				root = word;
			}
			else if(line == "index") {
				ssline >> word;
				indexx = word;
			}
			else if(line == "methods") {
				while(ssline >> line)
					methods += line;
			}
			else if(line == "}")
				return ;
			//std::cout << line << std::endl;
		}
	}
}

void create_server(std::stringstream &file)
{
	Serv serv;
	int number;
	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream ssline(line);

		while(ssline >> line)
		{
			if (line == "listen") {
				ssline >> number;
				serv.port_setter(number);
			}
			else if(line == "location")
			{
				ssline >> location;
				parse_location(file);
			}
			else if(line == "}")
				return ;
			//std::cout << line << std::endl;
		}
	}
}


void main_loop()
{
	std::string line;
	std::ifstream conf("../webserv.conf");
	if (conf.fail())
		std::cerr << "Config file: Error: " << strerror(errno);

	std::stringstream file;
	file << conf.rdbuf();
	while (std::getline(file, line))
	{
		if (line.find("server"))
			create_server(file);

		std::cout << location << std::endl;
		std::cout << methods << std::endl;
		std::cout << indexx << std::endl;
		std::cout << root << std::endl;
		//std::cout << location << std::endl;
	}

}
