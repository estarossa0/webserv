#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/poll.h>
#include <fcntl.h>

class	Server
{
	int					fd;
	struct sockaddr_in	addr;
	int					http_socket;
public:
	Server(int);

	int		connect();
	int		read(void *request, size_t size) { return recv(http_socket , request, size, 0);}
	int		send(void *response, size_t size) { return ::send(http_socket, response, size, 0);}
	int		close() { return ::close(http_socket);}
	int		get_fd() const { return fd;}
};

#define log std::cout <<
#define line << std::endl
