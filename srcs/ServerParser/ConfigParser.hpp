#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <iostream>
#include <exception>
#include <stdexcept>
#include <cctype>
#include <map>
#include <fstream>
#include <sstream>
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
#define ERROR_EMPTY_SERVER_CONFIGURATION "A server must not have an empty configuration"
#define ERROR_INVALID_CONFIGURATION "This configuration file is invalid: ERROR in this line -> "
#define ERROR_EMPTY_CONFIGURATION "Your file does not contains any server configuration"
#define ERROR_MISSING_SEMICOLON "missing a semicolon in this line: "
#define ERROR_PORT_NAN "the value of port should be a number" 

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

		// methods
		void _trim(std::string &);
		std::vector<std::string> _split(std::string const &);
		bool _isSet(std::string const &, int (*func)(int ));
		void _getFileContent();
		void _indexServers();
		int _isPrimitive(std::string const &);
		// partial parsers
		int _portParser(size_t, Server &);
		void _parseContent();

	public:
		ConfigParser(char const *inFilename);
		std::vector<Server> const &getServers() const;
		~ConfigParser();
		static std::string const primitives_openings[NUMBER_OF_SERVER_PRIMITIVES];
	};

	typedef  int (ConfigParser::*ParserFuncPtr)(size_t, Server &);
}
#endif // !CONFIG_PARSER_HPP //Config