#include "websrv.h"

int main(int ac, char **av)
{
	ConfigParser *parser;
	try {
		if (ac == 2)
			parser = new ConfigParser(av[1]);
		else
			parser = new ConfigParser("default_path");
		// Webserv web;

		// web.addServer(80);
		// web.hook();
	} catch (std::exception &e)
	{
		log e.what() line;
	}
	return 0;
}