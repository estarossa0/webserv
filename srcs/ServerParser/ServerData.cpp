#include "ServerData.hpp"

ServerData::ServerData()
{
}

ServerData::~ServerData()
{
	_error_pages.clear();
}

void ServerData::setPort(int const &p)
{
	_port = p;
}

int const &ServerData::getPort() const
{
	return _port;
}

void ServerData::setHost(std::string const &h)
{
	_host = h;
}

std::string const &ServerData::getHost() const
{
	return _host;
}

void ServerData::setName(std::string const &n)
{
	_name = n;
}

std::string const &ServerData::getName() const
{
	return _name;
}

void ServerData::setClientBodySize(int const &cbz)
{
	_client_body_size = cbz;
}

int const &ServerData::getClientBodySize() const
{
	return _client_body_size;
}

void ServerData::addErrorPage(int const &code, std::string const &path)
{
	if (path.empty())
		throw std::invalid_argument("empty argument");
	_error_pages.insert(std::pair<int, std::string>(code, path));
}

std::map<int, std::string> const &ServerData::getErrorPageMap() const
{
	return _error_pages;
}

void ServerData::setRootDir(std::string const &rd)
{
	_root_dir = rd;
}

std::string const &ServerData::getRootDir() const
{
	return _root_dir;
}

void ServerData::addLocation(Location const &loc)
{
	_locations.push_back(loc);
}

std::vector<Location> const &ServerData::getLocations() const
{
	return _locations;
}

std::ostream &operator<<(std::ostream &out, const ServerData &sv)
{
	out << "\n==================== ServerData ===================" << std::endl;
	out << "port :[" << sv.getPort() << "]" << std::endl;
	out << "host :[" << sv.getHost() << "]" << std::endl;
	out << "name :[" << sv.getName() << "]" << std::endl;
	out << "client body size: [" << sv.getClientBodySize() << "]" << std::endl;
	std::map<int, std::string> map(sv.getErrorPageMap());
	for (std::map<int, std::string>::iterator it = map.begin(); it != map.end(); it++)
	{
		out << "error page: code [" << it->first << "], path [" << it->second << "]" << std::endl;
	}
	out << "root dir: [" << sv.getRootDir() << "]" << std::endl;
	std::vector<Location> const & locs = sv.getLocations();
	for (size_t i = 0; i < locs.size(); i++)
	{
		out << locs[i];
	}
	out << "==================== ServerData End =================" << std::endl;
	return out;
}
