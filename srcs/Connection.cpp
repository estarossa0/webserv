#include "websrv.h"

Connection::Connection(int fd, Server *sv) : _socketfd(fd), _server(sv){};

Connection::Connection(const Connection &other) : _socketfd(other._socketfd), _server(other._server){};

int Connection::read()
{
	char buffer[1000] = {0};
	int retval;
	int size;

	size = 0;
	retval = 1;
	while (retval > 0)
	{
		bzero(buffer, 1000);
		retval = recv(this->_socketfd, (void *)&buffer, 1000, 0);
		if (buffer[0])
			_request.parseRequest(buffer);
		size += retval;
		if (retval < 1000)
			break;
	}
	return size;
}

int Connection::send()
{
	this->_response.setRequest(this->_request);
	this->_response.makeResponse();
	this->_request.clear();
	return ::send(this->_socketfd, (void *)this->_response.getResponse().c_str(), this->_response.getResponse().length(), 0);
}

Request &Connection::getRequest()
{
	return this->_request;
}

Response &Connection::getResponse()
{
	return this->_response;
}

void Connection::setRequest(Request request)
{
	this->_response = request;
}

void Connection::setResponse(Response response)
{
	this->_response = response;
}

void Connection::close()
{
	::close(this->_socketfd);
}

Connection::operator int()
{
	return this->_socketfd;
}
