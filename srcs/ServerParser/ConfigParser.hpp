#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <iostream>
#include <exception>
#include <stdexcept>
#include <cctype>
#include <map>
#include <fstream>
#include <algorithm>

#include "Server.hpp"

namespace parser
{

// server file configuration requirements openings
#define SERVER_OP "server"
#define PORT_OP "listen"
#define HOST_OP "host"
#define SERVER_NAME_OP "server_name"
#define CLIENT_MAX_SIZE_BODY_OP "client_max_body_size"
#define ERROR_PAGE_OP "error_page"
#define ROOT_OP "root"
#define LOCATION_OP "location"
#define RETURN_LOCATION_OP "location /return"
#define CGI_LOCATION_OP "location /cgi"
#define UPLOAD_LOCATION_OP "location /upload"

#define NUMBER_OF_SERVER_PRIMITIVES 10

#define OPENNING_BRACE "{"
#define CLOSING_BRACE "}"

// error messages
#define ERROR_FILE "Could not open configuration file"
#define ERROR_BRACES "Curly braces are not written well in the configuration file"
#define ERROR_DOUBLE_BRACE "Only one curly brace is allowed per line"
#define ERROR_BRACE_NOT_ALONE "the line which contains a curly brace must not contain something else"
#define ERROR_DEFINE_SERVER_INSIDE_SERVER "You can't define a server inside another server"
#define ERROR_EMPTY_CONFIGURATION "A server must not have an empty configuration"


	class ConfigParser
	{
	private:
		// attributes
		char const *_filename;
		std::vector<Server> _servers;
		int _serversNumber;
		std::vector<std::string> _fileLines;
		std::vector<int> _serversIndexing;
		std::map<std::string, bool> _checked_primitives;
		void (*_server_primitive_parser[NUMBER_OF_SERVER_PRIMITIVES])(size_t start, size_t end);

		// methods
		void _trim(std::string &);
		void _getFileContent();
		void _indexServers();
		void _initParsersFuntions();
		// partial parsers
		void _parseContent();

	public:
		ConfigParser(char const *inFilename);
		std::vector<Server> const &getServers() const;
		~ConfigParser();
		static std::string const primitives_openings[NUMBER_OF_SERVER_PRIMITIVES];
	};

}
#endif // !CONFIG_PARSER_HPP //Config