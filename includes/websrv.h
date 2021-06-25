#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "Connection.hpp"
#include "Server.hpp"

static void		hookPollIn(Webserv &, size_t);
static void		hookPollOutn(Webserv &, size_t);
class Webserv
{
	friend class					Server;
	std::vector<Server>				_servers;
	std::vector<int>				_indexTable;
	size_t							_conSize;

public:
	Webserv();
	~Webserv();

	void		hook();
	Server		&serverAt(int);
	void		addServer(int);
	Connection	&operator[](int);
	void		updateIndexs(int, int);

	std::vector <struct pollfd>		_pollArray;

	friend void	hookPollIn(Webserv &, size_t);
	friend void	hookPollOot(Webserv &, size_t);
};

#define log std::cout <<
#define line << std::endl
