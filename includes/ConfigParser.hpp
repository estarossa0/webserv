#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "websrv.h"

// fields identifiers fields definitions

// server file configuration requirements openings
#define SERVER_OP "server"
#define PORT_OP "listen"
#define HOST_OP "host"
#define SERVER_NAME_OP "server_name"
#define CLIENT_MAX_SIZE_BODY_OP "client_max_body_size"
#define ERROR_PAGE_OP "error_page"
#define ROOT_OP "root"
#define LOCATION_OP "location"

// location content fields identifiers
#define LOC_PATH "loc_path"
#define LOC_ROOT "root"
#define LOC_AUTOINDEX "autoindex"
#define LOC_INDEX "index"
#define LOC_ALLOWED_METHODS "allow_methods"
#define LOC_RETURN "return"
#define LOC_CGI "fastcgi_pass"
#define UPLOAD_LOC_ENABLE "upload_enable"
#define UPLOAD_LOC_STORE "upload_store"

#define NUMBER_OF_SERVER_PRIMITIVES 7
#define NUMBER_OF_LOCATION_PRIMITIVES 8

#define OPENNING_BRACE "{"
#define CLOSING_BRACE "}"

#define OPENNING_BRACKET '['
#define CLOSING_BRACKET ']'

// error messages
#define ERROR_FILE "Could not open configuration file"
#define ERROR_FILE_EXTENSION "Configuration file has not the correct extension [.conf]"
#define ERROR_BRACES "Curly braces are not written well in the configuration file or a server identifier used with no definition"
#define ERROR_OPENING_BRACE_WITHOUT_SERVER_OR_LOC "opening curly brace used without setting server or location identifier above it."
#define ERROR_DOUBLE_BRACE "Only one curly brace is allowed per line"
#define ERROR_BRACE_NOT_ALONE "the line which contains a curly brace must not contain something else.\nError in this line -> "
#define ERROR_DEFINE_SERVER_INSIDE_SERVER "You can't define a server inside another server"
#define ERROR_EMPTY_SERVER_CONFIGURATION "A server must not have an empty configuration"
#define ERROR_INVALID_CONFIGURATION "This configuration file is invalid: ERROR in this line -> "
#define ERROR_EMPTY_CONFIGURATION "Your file does not contains any server configuration"
#define ERROR_MISSING_NECESSARY_ELEMENT "Make sure that all servers in file configuration contains at least these 4 elements: [port, host, name, root]"
#define ERROR_MISSING_SEMICOLON "missing a semicolon in this line: "
#define ERROR_DOUBLE_SEMICOLON "should be only one semicolon at the end of this line: "
#define ERROR_PORT_NAN "the value of port must be a non-zero positive number"
#define ERROR_CLIENT_BODY_SIZE_UNITY "the client max body size must end with 'm' (refers to megabytes) as its unity"
#define ERROR_CLIENT_BODY_SIZE_NAN "the value of client max body size must be a non-zero positive number"
#define ERROR_ERRPAGE_CODE_NAN "the value of an error page code must be a non-zero positive number"
#define ERROR_ALLOWED_METHODS_SYNTAX "bad syntax for allowed methods in line: "
#define ERROR_ALLOWED_METHOD_METHOD_NOT_FOUND "this method is not one of the webserv allowed methods: [ GET, POST, DELETE ] \nError in this line: "
#define ERROR_SERVER_DUPLICATE_FIELD "duplicate server field in this line -> "
#define ERROR_LOCATION_DUPLICATE_FIELD "duplicate location field in this line -> "
#define ERROR_EMPTY_LOCATION_CONFIG "the file configuration has an empty location configuration for this location-> "
#define ERROR_LOCATION_WITH_SEMICOLON "location field does not end with a semicolon: error in this line -> "
#define ERROR_RETURN_CODE_NAN "the value of redirection code must be a non-zero positive number"
#define ERROR_CGI_EXTENSION_ERROR "the cgi extension is invalid, it must be in this format: /path/*.extention , e.g. /*.php or /path/*.extension \nError in this line: "
#define ERROR_CGI_NOT_FOUND "the fastcgi_pass field is not found after setting the cgi extension"
#define DID_YOU_MEAN "did you mean "
#define IN_THIS_LINE " field in this line -> "
#define ERROR_DUPLICATE_SERVER_NAME "try to use a unique name for each server: duplicate name -> "
#define ERROR_DUPLICATE_SERVER_HOST_AND_PORT "two servers cannot have the same host and port, at least one must differ.\nduplicate host and port: "
#define ERROR_CGI_LOCATION_PATH "path of cgi location is invalid, error in this line-> "
#define ERROR_INVALID_IDENTIFIER "invalid identifier: in this line -> "

class ConfigParser
{
private:
	// attributes
	char const *_filename;
	std::vector<ServerData> _servers;
	std::vector<std::string> _fileLines;
	std::vector<int> _serversIndexing;
	std::map<std::string, bool> _checked_primitives;
	std::map<std::string, bool> _checked_location_primitives;

	// methods
	void _trim(std::string &);
	std::vector<std::string> _split(std::string const &);
	std::vector<std::string> _split(std::string const &, char);
	bool _isSet(std::string const &, int (*func)(int));
	std::string const &_removeDuplicateChar(std::string &, char const);
	void _semicolonChecker(std::string &);
	void _getFileContent();
	void _indexServers();
	int _isPrimitive(std::string const &);
	int _isLocationPrimitive(std::string const &);

	// partial server fields parsers
	int _portParser(size_t, ServerData &);
	int _hostParser(size_t, ServerData &);
	int _serverNameParser(size_t, ServerData &);
	int _clientBodySizeParser(size_t, ServerData &);
	int _errorPageParser(size_t, ServerData &);
	int _rootDirParser(size_t, ServerData &);

	// partial server location fields parsers
	void _locationPathParser(size_t &, Location &);
	void _locRootDirParser(size_t, Location &);
	void _locAutoIndexParser(size_t, Location &);
	void _locIndexParser(size_t, Location &);
	void _locAllowedMethodsParser(size_t, Location &);
	void _locRedirectionParser(size_t, Location &);
	void _locUploadEnableParser(size_t, Location &);
	void _locUploadLocationParser(size_t, Location &);
	void _locCGIParser(size_t, Location &);

	int _locationParser(size_t, ServerData &);

	void _parseContent();

public:
	ConfigParser(char const *inFilename);
	std::vector<ServerData> const &getServers() const;
	void addServer(ServerData const &);
	~ConfigParser();
	static std::string const primitives_openings[NUMBER_OF_SERVER_PRIMITIVES];
	static std::string const location_identifiers[NUMBER_OF_LOCATION_PRIMITIVES];
};

typedef int (ConfigParser::*ParserFuncPtr)(size_t, ServerData &);
typedef void (ConfigParser::*LocationFieldParserFuncPtr)(size_t, Location &);

#endif // !CONFIG_PARSER_HPP