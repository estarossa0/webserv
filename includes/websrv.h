#pragma once

#include <vector>
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

#define log std::cout <<
#define line << std::endl
