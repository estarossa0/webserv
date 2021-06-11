#pragma once

#include "websrv.h"

class	Server;

class	Connection
{
	friend class	Webserv;
	friend class	Server;
	int				_socketfd;
	Server			*_server;
	Request			_request;
	Response		_response;
	bool			_is_Server;

public:
	Connection(int, Server *, bool);
	Connection(const Connection &);

	int					read();
	int					send();
	Server				*getServer();
	Request				&getRequest();
	Response			&getResponse();
	void				setRequest(Request);
	void				setResponse(Response);
	void				close();
	bool				is_Server();
	operator			int();
};
