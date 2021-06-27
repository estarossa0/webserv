#pragma once

#include "Webserv.hpp"

class Webserv;

class	Server
{
	friend class				Webserv;
	int							_socketfd;
	std::vector<Connection>		_connections;
	size_t						_index;
	Webserv						*_webserv;
	std::vector<ServerData const>	_data;

public:
	Server(ServerData const &, size_t, Webserv *);
	~Server();
	Server(Server const &);

	int				connect();
	int				get_fd();
	void			erase(int);
	Connection		&operator[](int);
	operator		int();
	size_t			size();

	std::vector<ServerData const> const &	getData() const;
	void			addData(ServerData const &data);
};
