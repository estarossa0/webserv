#pragma once

#include "websrv.h"

class 	Response;
class 	Request;
class	Server;

class	Connection
{
	int				_socketfd;
	const Server	*_server;
	Request			_request;
	Response		_response;

public:
	Connection(int, Server *);
	Connection(const Connection &);
	int					read();
	int					send();
	Request				&getRequest();
	Response			&getResponse();
	void				setRequest(Request);
	void				setResponse(Response);
	void				close();
	operator			int();
};
