#include "Server.hpp"

parser::Server::Server()
{
}

parser::Server::~Server()
{
	_error_pages.clear();
}

void parser::Server::setPort(int const &p)
{
	_port = p;
}

int const &parser::Server::getPort() const
{
	return _port;
}

void parser::Server::setHost(std::string const &h)
{
	_host = h;
}

std::string const &parser::Server::getHost() const
{
	return _host;
}

void parser::Server::setName(std::string const &n)
{
	_name = n;
}

std::string const &parser::Server::getName() const
{
	return _name;
}

void parser::Server::setClientBodySize(int const &cbz)
{
	_client_body_size = cbz;
}

int const &parser::Server::getClientBodySize() const
{
	return _client_body_size;
}

void parser::Server::addErrorPage(int const &code, std::string const &path)
{
	if (path.empty())
		throw std::invalid_argument("empty argument");
	_error_pages.insert(std::pair<int, std::string>(code, path));
}

std::map<int, std::string> const &parser::Server::getErrorPageMap() const
{
	return _error_pages;
}

void parser::Server::setRootDir(std::string const &rd)
{
	_root_dir = rd;
}

std::string const &parser::Server::getRootDir() const
{
	return _root_dir;
}

		
void parser::Server::addLocation(Location const &loc)
{
	_locations.push_back(loc);
}

std::vector<parser::Location> const &parser::Server::getLocations() const
{
	return _locations;
}
