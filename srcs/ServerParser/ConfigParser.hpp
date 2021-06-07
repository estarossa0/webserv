#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <iostream>
#include <vector>
namespace parser
{

// server file configuration requirements openings
#define PORT_OP "listen"
#define SERVER_NAME_OP "server_name"
#define CLIENT_MAX_SIZE_BODY_OP "client_max_body_size"
#define ERROR_PAGE_OP "error_page"
#define ROOT_OP "root"
#define LOCATION_OP "location /"
#define RETURN_LOCATION_OP "location /return"
#define CGI_LOCATION_OP "location /cgi"
#define UPLOAD_LOCATION_OP "location /upload"

	struct error_page
	{
		short error_code;
		std::string path;
	};

	struct location {
		std::string root_dir;
		bool autoindex;
		std::vector<std::string> files;
		std::vector<std::string> allowed_methods;
	};

	struct return_location {
		int code;
		std::string url;
	};

	struct cgi_location {

	};

	struct upload_location {
		
	};

	class Server
	{
	private:
		int port;
		std::string name;
		int client_body_size;
		std::vector<error_page> error_pages;
		std::string root_dir;
		location location;
		return_location return_location;
		cgi_location cgi_location;



	};

	class ConfigParser
	{
	private:
		char const *filename;

		// std::vector<>

	public:
		ConfigParser(char const *inFilename);
		~ConfigParser();
	};

}
#endif // !CONFIG_PARSER_HPP //Config