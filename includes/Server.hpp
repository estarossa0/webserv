#pragma once

#include "websrv.h"

class	Server
{
	int							_socketfd;
	std::vector<Connection>		_connections;
	struct sockaddr_in			_addr;
public:
	Server(int);
	~Server();
	int				connect();
	int				get_fd();
	void			erase(int);
	Connection		&operator[](int);
	operator		int();
};