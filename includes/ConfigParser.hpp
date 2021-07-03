#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "Webserv.hpp"

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

#define PHP_EXTENTION ".php"
#define PYTHON_EXTENTION ".py"

#define LOCALHOST "127.0.0.1"

// error messages
#define ERROR_FILE "Could not open configuration file"
#define ERROR_FILE_EXTENSION "Configuration file has not the correct extension [.conf]"
#define ERROR_BRACES "Curly braces are not written well in the configuration file or a server identifier used with no definition"
#define ERROR_OPENING_BRACE_WITHOUT_SERVER_OR_LOC "Opening curly brace used without setting server or location identifier above it."
#define ERROR_DOUBLE_BRACE "Only one curly brace is allowed per line"
#define ERROR_BRACE_NOT_ALONE "The line which contains a curly brace must not contain something else: Error in this line -> "
#define ERROR_DEFINE_SERVER_INSIDE_SERVER "You can't define a server inside another server"
#define ERROR_EMPTY_SERVER_CONFIGURATION "A server must not have an empty configuration"
#define ERROR_INVALID_CONFIGURATION "This configuration file is invalid: ERROR in this line -> "
#define ERROR_EMPTY_CONFIGURATION "Your file does not contains any server configuration"
#define ERROR_MISSING_ELEMENTS " necessary missing elements: "
#define ERROR_MISSING_SEMICOLON "Missing a semicolon in this line: "
#define ERROR_DOUBLE_SEMICOLON "Should be only one semicolon at the end of this line: "
#define ERROR_PORT_NAN "The port value must be a positive number"
#define ERROR_CLIENT_BODY_SIZE_UNITY "The client max body size must end with 'm' (refers to megabytes) as its unity"
#define ERROR_CLIENT_BODY_SIZE_NAN "The value of client max body size must be a non-zero positive number"
#define ERROR_ERRPAGE_CODE_NAN "The value of an error page code must be a non-zero positive number"
#define ERROR_ALLOWED_METHODS_SYNTAX "Bad syntax for allowed methods in line: "
#define ERROR_ALLOWED_METHOD_METHOD_NOT_FOUND "This method is not one of the webserv allowed methods: [ GET, POST, DELETE ], Error in this line: "
#define ERROR_SERVER_DUPLICATE_FIELD "Duplicate server field in this line -> "
#define ERROR_LOCATION_DUPLICATE_FIELD "Duplicate location field in this line -> "
#define ERROR_EMPTY_LOCATION_CONFIG "The file configuration has an empty location configuration for this location-> "
#define ERROR_LOCATION_WITH_SEMICOLON "Location field does not end with a semicolon: error in this line -> "
#define ERROR_RETURN_CODE_NAN "The value of redirection code must be a non-zero positive number"
#define ERROR_CGI_EXTENSION_ERROR "The CGI extension is invalid, it must be in this format: *.extention , e.g. *.php, *.py, Error in this line: "
#define ERROR_CGI_NOT_FOUND "The fastcgi_pass field is not found after setting the cgi extension"
#define DID_YOU_MEAN "Did you mean "
#define IN_THIS_LINE " field in this line -> "
#define ERROR_DUPLICATE_SERVER_NAME "Try to use a unique name for each server: duplicate name -> "
#define ERROR_DUPLICATE_SERVER_HOST_AND_PORT "Two servers cannot have the same host and port, at least one must differ.duplicate host and port: "
#define ERROR_INVALID_IDENTIFIER "Invalid identifier: in this line -> "
#define CGI_NOT_SUPPORTED "Only these extensions are supported for CGI: [.php] and [.py], Error in this line:[ location "

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
	void _parseArguments(int ac, char *av[]);

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
	bool _isCGIsupportedExtension(std::string const &);
	void _locCGIParser(size_t, Location &);

	int _locationParser(size_t, ServerData &);

	void _parseContent();

	void addServer(ServerData const &);

public:
	ConfigParser(int ac, char *av[]);
	std::vector<ServerData> getServers() const;
	~ConfigParser();
	static std::string const primitives_openings[NUMBER_OF_SERVER_PRIMITIVES];
	static std::string const location_identifiers[NUMBER_OF_LOCATION_PRIMITIVES];
};

typedef int (ConfigParser::*ParserFuncPtr)(size_t, ServerData &);
typedef void (ConfigParser::*LocationFieldParserFuncPtr)(size_t, Location &);

#endif // !CONFIG_PARSER_HPP
