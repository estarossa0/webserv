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
	std::vector<ServerData>		_data;
	std::map<std::string, std::vector<ServerData> >	_namesTable;
	int							_port;

public:
	Server(ServerData const &, size_t, Webserv *);
	~Server();
	Server(Server const &);

	int				connect();
	int				get_fd();
	int				getPort();
	void			erase(int);
	Connection		&operator[](int);
	operator		int();
	size_t			size();

	std::vector<ServerData> 	const &	getData(std::string &) ;
	void			addData(ServerData const &data);
};
