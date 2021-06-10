#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <exception>
#include <stdexcept>
#include <cctype>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>

#include "Location.hpp"

namespace parser
{
	class Server
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


	public:
		Server();
		~Server();

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

	};


} // namespace parser

#endif // !SERVER_HPP