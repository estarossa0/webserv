#ifndef SERVER_PRIMITIVES_HPP
#define SERVER_PRIMITIVES_HPP

#include <string>
#include <vector>

struct location
{
	std::string location_path;
	std::string root_dir;
	bool autoindex;
	std::vector<std::string> files;
	std::vector<std::string> allowed_methods;
};

struct return_location
{
	int code;
	std::string url;
};

struct cgi_location
{
};

struct upload_location
{
	bool upload_enable;
};


#endif // !SERVER_PRIMITIVES_HPP