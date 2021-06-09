#pragma once

#include "websrv.h"

class	Server;

class	Connection
{
	friend class	Webserv;
	friend class	Server;
	int				_socketfd;
	Server			*_server;
	std::string		_request;
	std::string		_response;

public:
	Connection(int, Server *);
	Connection(const Connection &);

	int					read();
	int					send();
	Server				*getServer();
	std::string			&getRequest();
	std::string			&getResponse();
	void				setRequest(char *);
	void				setResponse(char *);
	void				close();
	operator			int();
};
