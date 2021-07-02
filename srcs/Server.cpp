#include "Webserv.hpp"

Server::Server(ServerData const &data, size_t index, Webserv *wb) : _index(index), _webserv(wb), _port(data.getPort())
{
	int	opt(1);
	struct sockaddr_in	addr;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(data.getHost().c_str());
	addr.sin_port = htons(_port);

	if ((this->_socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw std::runtime_error(std::string("WARNING server <")
			+ data.getHost()+ ":" + std::to_string(_port) + "> " + std::string(strerror(errno)) + "; SKIPPED");

	if (setsockopt(this->_socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
		throw std::runtime_error(std::string("WARNING server <")
			+ data.getHost()+ ":" + std::to_string(_port) + "> " + std::string(strerror(errno)) + "; SKIPPED");

	if (bind(this->_socketfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		throw std::runtime_error(std::string("WARNING server <")
			+ data.getHost() + ":" + std::to_string(_port) + "> " + std::string(strerror(errno)) + "; SKIPPED");

	if (listen(this->_socketfd, 3) < 0)
		throw std::runtime_error(std::string("WARNING server <")
			+ data.getHost()+ ":" + std::to_string(_port) + "> " + std::string(strerror(errno)) + "; SKIPPED");

	log "Binding on " + data.getHost()+ ":" + std::to_string(_port) line;
	this->addData(data);
	this->_connections.push_back(Connection(this->_socketfd, this, true, addr));
	this->_webserv->_pollArray.push_back((struct pollfd){this->_socketfd, POLLIN, 0});
}

Server::~Server()
{}

Server::Server(Server const &other) :
_socketfd(other._socketfd), _index(other._index), _webserv(other._webserv), _data(other._data), _namesTable(other._namesTable), _port(other._port)
{
	this->_connections = other._connections;
	for (std::vector<Connection>::iterator it = this->_connections.begin(); it != this->_connections.end(); ++it)
		it->_server = this;
}

int		Server::connect()
{
	int	newfd;
	struct sockaddr_in	client_addr;
	int	len(sizeof(client_addr));

	newfd = accept(this->_socketfd, (struct sockaddr *)&client_addr, (socklen_t*)&len);
	if (newfd < 0)
	{
		throw std::runtime_error(std::string("Error with client <")
			+ inet_ntoa(client_addr.sin_addr) + ":" + std::to_string(ntohs(client_addr.sin_port))
			+ "> " + std::string(strerror(errno)));
	}
	log "Accepted connection from " + std::string(inet_ntoa(client_addr.sin_addr))
		+ ":" + std::to_string(ntohs(client_addr.sin_port)) line;
	this->_connections.push_back(Connection(newfd, this, false, client_addr));
	this->_webserv->_pollArray
	.insert(
		this->_webserv->_pollArray.begin() + this->_webserv->_indexTable[this->_index] + (this->_connections.size() - 1),
		(struct pollfd){newfd, POLLIN, 0});
	this->_webserv->updateIndexs(this->_index, 1);
	return newfd;
}

int		Server::get_fd()
{
	return this->_socketfd;
}

int		Server::getPort()
{
	return this->_port;
}

void	Server::erase(int index)
{
	log "Closing connection with " + std::string(inet_ntoa(this->_connections[index]._addr.sin_addr))
		+ ":" + std::to_string(ntohs(this->_connections[index]._addr.sin_port)) line;
	this->_webserv->_pollArray.erase(this->_webserv->_pollArray.begin() + index);
	this->_webserv->updateIndexs(this->_index, -1);
	index -= this->_webserv->_indexTable[this->_index];
	this->_connections[index].close();
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

std::vector<ServerData>const &	Server::getData(std::string &name)
{
	std::map<std::string, std::vector<ServerData> >::const_iterator it;

	it = this->_namesTable.find(name);
	if (it != this->_namesTable.end())
		return this->_namesTable[name];
	return this->_data;
}

void		Server::addData(ServerData const &data)
{
	this->_data.push_back(data);
	for (std::vector<std::string const>::iterator it = data.getNames().begin(); it != data.getNames().end(); ++it)
	{
		this->_namesTable[*it].push_back(data);
	}
}