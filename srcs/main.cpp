#include "Webserv.hpp"
// #include <iostream>

int main(int ac, char **av)
{
	ConfigParser *parser = nullptr;
	try {
		outputLogs("\n\n-- starting new webserv session --");
		Webserv web;
		if (ac == 2)
			parser = new ConfigParser(av[1]);
		else
			parser = new ConfigParser("webserv.conf");
		web.init(parser->getServers());
		web.hook();
		outputLogs("-- ending the webserv session successfuly --");
	} catch (std::exception &e)
	{
		log e.what() line;
		try{
			outputLogs("error: " + std::string(e.what()));
			outputLogs("-- ending the webserv session with an error --");
		} catch (std::exception &e) {}
	}
	delete parser;

	return 0;
}
