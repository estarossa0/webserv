#include "websrv.h"

Connection::Connection(int fd, Server* sv) : _socketfd(fd), _server(sv), _request(""), _response("")
{};

Connection::Connection(const Connection &other) : _socketfd(other._socketfd), _server(other._server) , _request(""), _response("")
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
		retval = recv(this->_socketfd , (void *)&buffer, 1000, 0);
		size += retval;
		this->_request += buffer;
		if (retval < 1000)
			break ;
	}
	return size;
}

int				Connection::send()
{
	return ::send(this->_socketfd, (void *)this->_response.c_str(), this->_response.size(), 0);
}

std::string		&Connection::getRequest()
{
	return this->_request;
}

std::string		&Connection::getResponse()
{
	return this->_response;
}

void			Connection::setRequest(char *str)
{
	this->_request = str;
}

void			Connection::setResponse(char *str)
{
	this->_response = str;
}

void			Connection::close()
{
	::close(this->_socketfd);
}

Connection::operator int()
{
	return this->_socketfd;
}
