#include "websrv.h"

int main()
{
	Webserv	web;

	web.addServer(3030);
	web.hook();
}