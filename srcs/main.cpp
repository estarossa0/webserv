#include "Webserv.hpp"

int main(int ac, char **av)
{
	try
	{
		outputLogs("\n\n-- starting new webserv session --");
		ConfigParser parser(ac, av);
		Webserv web;
		web.init(parser.getServers());
		web.hook();
		outputLogs("-- ending the webserv session successfuly --");
	}
	catch (std::exception &e)
	{
		log e.what() line;
		outputLogs("Error: " + std::string(e.what()));
		outputLogs("-- ending the webserv session with an error --");
		return 1;
	}
	return 0;
}
