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

#include "Request.hpp"
#include "Response.hpp"
#include "Connection.hpp"
#include "Server.hpp"


#define log std::cout <<
#define line << std::endl
