#include "Webserv.hpp"

Connection::Connection(int fd, Server* sv, bool isserv, struct sockaddr_in	cl_addr) : _socketfd(fd), _server(sv), _is_Server(isserv), _request(this), _response(this), _addr(cl_addr)
{};

Connection::Connection(const Connection &other) :
_socketfd(other._socketfd), _server(other._server), _is_Server(other._is_Server), _request(this), _response(this), _addr(other._addr)
{};

int				Connection::read()
{
	char	buffer[1001] = {0};
	int		retval;
	int		size;

	size = 0;
	retval = 1;
	while (retval > 0)
	{
		bzero(buffer, 1000);
		retval = recv(this->_socketfd, (void *)&buffer, 1000, 0);
		if (retval != -1)
		{
			buffer[retval] = '\0';
			_request.appendToData(buffer);
			size += retval;
		}
		if (_request.checkDataDone())
			break ;
	}
	return size;
}

int				Connection::send()
{
	if (DEBUG)
		this->_request.printRequest();
	this->_response.setRequest(this->_request);
	this->_response.makeResponse();
	if (DEBUG)
		outputLogs("[++++]  RESPONSE  [++++]\n" + this->_response.getResponse() + "[----]  END RESPONSE  [----]");
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

std::string		Connection::getIp() const
{
	return inet_ntoa(this->_addr.sin_addr);
}

int				Connection::getPort() const
{
	return ntohs(this->_addr.sin_port);
}
