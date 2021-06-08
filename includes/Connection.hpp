#pragma once

#include "websrv.h"

class	Server;

class	Connection
{
	int				_socketfd;
	const Server	*_server;
	std::string		_request;
	std::string		_response;

public:
	Connection(int, Server *);
	Connection(const Connection &);
	int					read();
	int					send();
	std::string			&getRequest();
	std::string			&getResponse();
	void				setRequest(char *);
	void				setResponse(char *);
	void				close();
	operator			int();
};
