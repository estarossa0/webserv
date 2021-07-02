#pragma once

#include "Webserv.hpp"

class	Server;

class	Connection
{
	friend class	Webserv;
	friend class	Server;
	int				_socketfd;
	Server			*_server;
	bool			_is_Server;
	Request			_request;
	Response		_response;
	struct sockaddr_in	_addr;

public:
	Connection(int, Server *, bool, struct sockaddr_in);
	Connection(const Connection &);
	Connection &operator=(const Connection &);

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
	std::string			getIp() const;
	int					getPort() const;
};
