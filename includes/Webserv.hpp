#pragma once

#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <fstream>
#include <limits.h>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <cctype>
#include <map>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <arpa/inet.h>
#include <cstdio>
#include <dirent.h>
#include <cstdlib>

#include "Location.hpp"
#include "ServerData.hpp"
#include "ConfigParser.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Connection.hpp"
#include "Server.hpp"

#define ST_OK 200
#define ST_MOVED_PERM 301
#define ST_BAD_REQUEST 400
#define ST_FORBIDDEN 403
#define ST_NOT_FOUND 404
#define ST_METHOD_NOT_ALLOWED 405
#define ST_PAYLOAD_LARGE 413
#define ST_URI_TOO_LONG 414
#define ST_SERVER_ERROR 500
#define ST_NOT_IMPLEMENTED 501
#define ST_BAD_GATEWAY 502

#define LOGS_FILE "webserv_logs"

extern char **environ;

#ifndef DEBUG
# define DEBUG 1
#endif

void		hookPollIn(Webserv &, size_t);
void		hookPollOut(Webserv &, size_t);

void outputLogs(std::string);

class Webserv
{
	friend class					Server;
	std::vector<Server>				_servers;
	std::vector<int>				_indexTable;
	size_t							_conSize;

public:
	Webserv();
	~Webserv();

	void		hook();
	Server		&serverAt(int);
	void		addServer(ServerData const &);
	Connection	&operator[](size_t);
	void		updateIndexs(int, int);
	void		init(std::vector<ServerData> const &);

	std::vector <struct pollfd>		_pollArray;

	friend void	hookPollIn(Webserv &, size_t);
	friend void	hookPollOot(Webserv &, size_t);
};

#define log std::cout <<
#define line << std::endl
