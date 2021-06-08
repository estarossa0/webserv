#pragma once

#include "websrv.h"

class 	Request;
class	Server;

class	Connection
{
	int				_socketfd;
	const Server	*_server;
	Request			_request;
	std::string		_response;

public:
	Connection(int, Server *);
	Connection(const Connection &);
	int					read();
	int					send();
	Request				&getRequest();
	std::string			&getResponse();
	void				setRequest(char *);
	void				setResponse(char *);
	void				close();
	operator			int();
};
