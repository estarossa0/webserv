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

int parser::Server::getPort() const
{
	return _port;
}

void parser::Server::setHost(std::string const &h)
{
	_host = h;
}

std::string parser::Server::getHost() const
{
	return _host;
}

void parser::Server::setName(std::string const &n)
{
	_name = n;
}

std::string parser::Server::getName() const
{
	return _name;
}

void parser::Server::setClientBodySize(int const &cbz)
{
	_client_body_size = cbz;
}

int parser::Server::getClientBodySize() const
{
	return _client_body_size;
}

void parser::Server::addErrorPage(int const &code, std::string const &path)
{
	if (path.empty())
		throw std::invalid_argument("empty argument");
	_error_pages.insert(std::pair<int, std::string>(code, path));
}

std::map<std::string, std::string> parser::Server::getErrorPageMap() const
{
	return _error_pages;
}

void parser::Server::setRootDir(std::string const &rd)
{
	_root_dir = rd;
}

std::string parser::Server::getRootDir() const
{
	return _root_dir;
}

void parser::Server::setLocation(location const &loc)
{
	_location = loc;
}

location parser::Server::getLocation() const
{
	return _location;
}

void parser::Server::setReturnLocation(return_location const &r_loc)
{
	_return_location = r_loc;
}

return_location parser::Server::getReturnLocation() const
{
	return _return_location;
}

void parser::Server::setCGILocation(cgi_location const &cgi_loc)
{
	_cgi_location = cgi_loc;
}

cgi_location parser::Server::getCGILocation() const
{
	return _cgi_location;
}

void parser::Server::setUploadLocation(upload_location const &upload_loc)
{
	_upload_location = upload_loc;
}

upload_location parser::Server::getUploadLocation() const
{
	return _upload_location;
}
