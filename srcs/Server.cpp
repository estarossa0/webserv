#include "websrv.h"

Server::Server(int port, size_t index) : _index(index)
{
	int	opt(1);

	this->_addr.sin_family = AF_INET;
	this->_addr.sin_addr.s_addr = INADDR_ANY;
	this->_addr.sin_port = htons(port);

	if ((this->_socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		std::cerr << "ERROR: " << std::strerror(errno) line;
		exit(EXIT_FAILURE);
	}

	if (setsockopt(this->_socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
	{
		std::cerr << "ERROR: " << std::strerror(errno) line;
		exit(EXIT_FAILURE);
	}

	if (bind(this->_socketfd, (struct sockaddr *)&this->_addr, sizeof(this->_addr)) == -1)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	if (listen(this->_socketfd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	this->_connections.push_back(Connection(this->_socketfd, this));
}

Server::~Server()
{
	for (std::vector<Connection>::iterator it = this->_connections.begin(); it != this->_connections.end(); it++)
	{
		it->close();
	}
}

Server::Server(Server const &other) :
_socketfd(other._socketfd), _addr(other._addr), _index(other._index), _webserv(other._webserv)
{
	this->_connections = other._connections;
	for (std::vector<Connection>::iterator it = this->_connections.begin(); it != this->_connections.end(); ++it)
		it->_server = this;
}

int		Server::connect()
{
	int	len(sizeof(_addr));
	int	newfd;

	newfd = accept(this->_socketfd, (struct sockaddr *)&this->_addr, (socklen_t*)&len);
	if (newfd < 0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
	this->_connections.push_back(Connection(newfd, this));
	return newfd;
}

int		Server::get_fd()
{
	return this->_socketfd;
}

void	Server::erase(int index)
{
	_connections.erase(_connections.begin() + index);
}

Connection &Server::operator[](int index)
{
	return _connections[index];
}

Server::operator int()
{
	return this->_socketfd;
}

size_t	Server::size()
{
	return this->_connections.size();
}
