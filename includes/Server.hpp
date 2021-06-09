#pragma once

#include "websrv.h"

class	Server
{
	int							_socketfd;
	std::vector<Connection>		_connections;
	struct sockaddr_in			_addr;
	size_t						_index;
public:
	Server(int, size_t);
	~Server();
	Server(Server const &);

	int				connect();
	int				get_fd();
	void			erase(int);
	Connection		&operator[](int);
	operator		int();
	size_t			size();
};