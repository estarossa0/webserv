#ifndef SERVER_DATA_HPP
#define SERVER_DATA_HPP

#include "Webserv.hpp"

#define NUMBER_OF_NECESSARY_ELEMENTS 3

#define PORT_NECESSITY_NUMBER 0
#define HOST_NECESSITY_NUMBER 1
#define ROOT_NECESSITY_NUMBER 2

class ServerData
{
private:
	std::vector<int> _ports;
	int _port;
	std::string _host;
	std::vector<std::string> _names;
	int _client_body_size;
	// error_pages<error code, path of html error page>
	std::map<int, std::string> _error_pages;
	std::string _root_dir;
	std::vector<Location> _locations;
	bool _necessary_elements[NUMBER_OF_NECESSARY_ELEMENTS];
	std::vector<std::string> _necessary_elements_identifiers;
	std::vector<std::string> hasNecessaryElements() const;
	void activateParsingMode();

	friend class ConfigParser;

public:
	ServerData();
	ServerData(ServerData const &);
	ServerData &operator=(ServerData const &);
	~ServerData();

	void setPort(int const &);
	int const &getPort() const;

	void addPort(int const &);
	std::vector<int> const &getPorts() const;

	void setHost(std::string const &);
	std::string const &getHost() const;

	void setNames(std::vector<std::string> const &);
	std::vector<std::string> const &getNames() const;

	void setClientBodySize(int const &);
	int const &getClientBodySize() const;

	void addErrorPage(int const &, std::string const &);
	std::map<int, std::string> const &getErrorPageMap() const;

	void setRootDir(std::string const &);
	std::string const &getRootDir() const;

	void addLocation(Location const &);
	std::vector<Location> const &getLocations() const;
};

std::ostream &operator<<(std::ostream &, const ServerData &);

#endif // !SERVER_DATA_HPP
