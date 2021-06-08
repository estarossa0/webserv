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

#include "server_primitives.hpp"

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
		std::map<std::string, std::string> _error_pages;
		std::string _root_dir;
		location _location;
		return_location _return_location;
		cgi_location _cgi_location;
		upload_location _upload_location;

	public:
		Server();
		~Server();
		void setPort(int const &);
		int getPort() const;
		void setHost(std::string const &);
		std::string getHost() const;
		void setName(std::string const &);
		std::string getName() const;
		void setClientBodySize(int const &);
		int getClientBodySize() const;
		void addErrorPage(std::string const &, std::string const &);
		std::map<std::string, std::string> getErrorPageMap() const;
		void setRootDir(std::string const &);
		std::string getRootDir() const;
		void setLocation(location const &);
		location getLocation() const;
		void setReturnLocation(return_location const &);
		return_location getReturnLocation() const;
		void setCGILocation(cgi_location const &);
		cgi_location getCGILocation() const;
		void setUploadLocation(upload_location const &);
		upload_location getUploadLocation() const;

	};


} // namespace parser

#endif // !SERVER_HPP //Config