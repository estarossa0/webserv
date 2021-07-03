#include "Webserv.hpp"

int main(int ac, char **av)
{
	Webserv web;
	try
	{
		ConfigParser parser(ac, av);
		web.init(parser.getServers());
		web.hook();
	}
	catch (std::exception &e)
	{
		web.logError(std::string(e.what()));
	}
	return web.success ? 0 : 1;
}
