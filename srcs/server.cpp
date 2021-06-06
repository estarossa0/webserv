#include "websrv.h"

server::server(int port)
{
	int	opt(1);

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if ((this->fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		std::cerr << "ERROR: " << std::strerror(errno) line;
		exit(EXIT_FAILURE);
	}

	if (setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
	{
		std::cerr << "ERROR: " << std::strerror(errno) line;
		exit(EXIT_FAILURE);
	}

	if (bind(this->fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	if (listen(this->fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
}

void	server::connect()
{
	int	len(sizeof(addr));

	if ((http_socket = accept(this->fd, (struct sockaddr *)&addr, (socklen_t*)&len))<0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
}