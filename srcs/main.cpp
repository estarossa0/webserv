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
		web.init(parser->getServers());
		web.hook();
	} catch (std::exception &e)
	{
		log e.what() line;
	}
	delete parser;

	return 0;
}
