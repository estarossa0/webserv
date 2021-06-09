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
	bool			_is_Server;

public:
	Connection(int, Server *, bool);
	Connection(const Connection &);

	int					read();
	int					send();
	Server				*getServer();
	std::string			&getRequest();
	std::string			&getResponse();
	void				setRequest(char *);
	void				setResponse(char *);
	void				close();
	bool				is_Server();
	operator			int();
};
