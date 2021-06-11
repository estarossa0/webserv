#ifndef SERVER_DATA_HPP
#define SERVER_DATA_HPP

#include "websrv.h"

#define NUMBER_OF_NECESSARY_ELEMENTS 3

#define PORT_NECESSITY_NUMBER 0
#define HOST_NECESSITY_NUMBER 1
#define ROOT_NECESSITY_NUMBER 2

class ServerData
{
private:
	int _port;
	std::string _host;
	std::string _name;
	int _client_body_size;
	// error_pages<error code, path of html error page>
	std::map<int, std::string> _error_pages;
	std::string _root_dir;
	std::vector<Location> _locations;
	bool _necessary_elements[NUMBER_OF_NECESSARY_ELEMENTS];

public:
	ServerData();
	~ServerData();

	void setPort(int const &);
	int const & getPort() const;

	void setHost(std::string const &);
	std::string const &getHost() const;

	void setName(std::string const &);
	std::string const & getName() const;
	
	void setClientBodySize(int const &);
	int const &getClientBodySize() const;
	
	void addErrorPage(int const &, std::string const &);
	std::map<int, std::string> const &getErrorPageMap() const;
	
	void setRootDir(std::string const &);
	std::string const &getRootDir() const;
	
	void addLocation(Location const &);
	std::vector<Location> const &getLocations() const;

	bool const hasNecessaryElements() const;

};

std::ostream &operator<<(std::ostream &, const ServerData &);

#endif // !SERVER_DATA_HPP