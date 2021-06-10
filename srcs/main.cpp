#include "websrv.h"

int main()
{
	Webserv web;

	web.addServer(80);
	web.hook();

	return 0;
}