#include "websrv.h"
// #include <iostream>

int main(int ac, char **av)
{
	ConfigParser *parser = nullptr;
	try {
		if (ac == 2)
			parser = new ConfigParser(av[1]);
		else
			parser = new ConfigParser("webserv.conf");
		// Webserv web;

		// web.addServer(80);
		// web.hook();
	} catch (std::exception &e)
	{
		log e.what() line;
	}
	delete parser;

	return 0;
}