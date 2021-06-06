#include "../includes/websrv.h"

// testing
#include <iostream>
#include <stdexcept>

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>

#define ADDRESS_FAMILY AF_INET
#define MY_PORT 26000


int main()
{
	int server_fd;
	sockaddr_in addr;

	// Creating socket
	if ((server_fd = socket(ADDRESS_FAMILY, SOCK_STREAM, 0)) < 0) 
	{
		throw std::runtime_error("cannot create socket"); 
		return 1; 
	}
	else
		std::cout << "Created socket successfully with fd = [" << server_fd << "]" << std::endl;

	// binding socket
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = ADDRESS_FAMILY;
	addr.sin_port = MY_PORT;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0)
	{
		throw std::runtime_error("cannot bind socket"); 
		return 1; 
	}
	else
		std::cout << "socket binded successfully with fd = [" << server_fd << "]" << std::endl;


}