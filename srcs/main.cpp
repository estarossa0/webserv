#include "../includes/websrv.h"

// testing
#include <iostream>
#include <stdexcept>

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>

#define ADDRESS_FAMILY AF_INET
#define PORT 80

// int main()
// {
// 	char buffer[1024] = {0};
// 	char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 5\r\n";
// 	Server sv(PORT);
// 	std::vector <struct pollfd> poll_set;


// 	poll_set.push_back((struct pollfd){sv.get_fd(), POLLIN, 0});int p;
// 	while (1)
// 	{
// 		bzero(buffer, 1024);
// 		p = poll(&poll_set[0], poll_set.size(), -1);
// 		if (p < 0)
// 			break ;
// 		for (size_t i = 0; i < poll_set.size(); i++)
// 		{
// 			if (poll_set[i].revents == 0)
// 				continue ;
// 			if ((poll_set[i].revents & POLLNVAL) && (poll_set[i].revents & POLLERR))
// 			{
// 				log "ERROR" line;
// 				exit(1);
// 			}
// 			if (poll_set[i].revents & POLLIN)
// 			{
// 				if (poll_set[i].fd == sv.get_fd())
// 				{
// 					poll_set.push_back((struct pollfd){sv.connect(), POLLIN, 0});
// 				}
// 				else
// 				{
// 					recv(poll_set[i].fd, buffer, sizeof(buffer), 0);
// 					poll_set[i].events = POLLOUT | POLLIN;
// 					if (poll_set[i].revents & POLLHUP)
// 					{
// 						close(poll_set[i].fd);
// 						poll_set.erase(poll_set.begin() + i);
// 					}
// 					log buffer line;
// 				}
// 			}
// 			if (poll_set[i].revents & POLLOUT)
// 			{
// 				fcntl(poll_set[i].fd, F_SETFL, O_NONBLOCK);
// 				send(poll_set[i].fd, (char *) response, strlen(response), 0);	// response header
// 				send(poll_set[i].fd, (void *)"\n", 1, 0);						//indicate end of header
// 				send(poll_set[i].fd, (void *)"hello", 5, 0);					// body message
// 				printf("Hello message sent\n");
// 				poll_set[i].events = POLLIN;
// 			}
// 		}
// 	}
// 	return 0;
// }


int main()
{
	std::string str = "Hello World";
	std::string str1 = str;

	std::cout << "[" << str1 << "] [" << str << "]" << std::endl;
	str1.erase(str1.end() - 1);
	std::cout << "[" << str1 << "] [" << str << "]" << std::endl;
}