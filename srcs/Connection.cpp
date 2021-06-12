#include "websrv.h"

Connection::Connection(int fd, Server* sv, bool isserv) : _socketfd(fd), _server(sv), _is_Server(isserv), _request(this), _response(this)
{};

Connection::Connection(const Connection &other) :
_socketfd(other._socketfd), _server(other._server), _is_Server(other._is_Server), _request(this), _response(this)
{};

int				Connection::read()
{
	char	buffer[1000] = {0};
	int		retval;
	int		size;

	size = 0;
	retval = 1;
	while (retval > 0)
	{
		bzero(buffer, 1000);
		retval = recv(this->_socketfd, (void *)&buffer, 1000, 0);
		if (retval > 0)
			_request.appendToData(buffer);
		size += retval;
		if (retval < 1000)
			break ;
		// if ((buffer[0] == '\r' && buffer[1] == '\n' ))
	}
	return size;
}

int				Connection::send()
{
	// this->_request.printRequest();
	this->_response.setRequest(this->_request);
	this->_response.makeResponse();
	log this->_response.getResponse() line;
	return ::send(this->_socketfd, (void *)this->_response.getResponse().c_str(), this->_response.getResponse().length(), 0);
}

Request		&Connection::getRequest()
{
	return this->_request;
}

Response		&Connection::getResponse()
{
	return this->_response;
}

void			Connection::setRequest(Request request)
{
	this->_request = request;
}

void			Connection::setResponse(Response response)
{
	this->_response = response;
}

void			Connection::close()
{
	::close(this->_socketfd);
}

Connection::operator int()
{
	return this->_socketfd;
}

Server				*Connection::getServer()
{
	return this->_server;
}

bool			Connection::is_Server()
{
	return this->_is_Server;
}
