#include "Webserv.hpp"
// #include <iostream>

int main(int ac, char **av)
{
	ConfigParser *parser = nullptr;
	try {
		Webserv web;
		if (ac == 2)
			parser = new ConfigParser(av[1]);
		else
			parser = new ConfigParser("default_path");
		// web.init(parser->getServers());
		std::map<int, std::vector<ServerData> > portsMap = parser->getPortsServerDataMap();
		for (std::map<int, std::vector<ServerData> >::iterator it = portsMap.begin(); it != portsMap.end(); it++)
		{
			log "Servers Data for port [" << it->first << "]: " line;
			std::vector<ServerData> vec = it->second;
			for (size_t i = 0; i < vec.size(); i++)
			{
				log vec[i] line;
			}
			
		}
			// web.hook();
	} catch (std::exception &e)
	{
		log e.what() line;
	}
	delete parser;

	return 0;
}
