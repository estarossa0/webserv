#include "websrv.h"
#define PORT 80

int main()
{
	int	server_fd, new_socket, valread;
	struct sockaddr_in address;
	char c;
	char buffer[1024] = {0};
	int opt = 1;
	char response[] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 5\n";
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
												&opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	int len = sizeof(address);
	while (1)
	{
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&len))<0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}
		valread = read( new_socket , buffer, 1024);
		log valread line;
		log buffer line;
		send(new_socket, response, strlen(response), 0); // response header
		send(new_socket , "\n" , 1 , 0 );				//indicate end of header
		send(new_socket , "hello" , 5 , 0 );			// body message
		printf("Hello message sent\n");
		close(new_socket);
	}
	return 0;
}