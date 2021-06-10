#include "websrv.h"

#define PORT 80
int main()
{
	char buffer[1024] = {0};
	Server sv(PORT);
	Request request;
	Response response;
	std::vector<struct pollfd> poll_set;
	poll_set.push_back((struct pollfd){sv.get_fd(), POLLIN, 0});
	int p;
	while (1)
	{
		bzero(buffer, 1024);
		p = poll(&poll_set[0], poll_set.size(), -1);
		if (p < 0)
			break;
		for (size_t i = 0; i < poll_set.size(); i++)
		{
			if (poll_set[i].revents == 0)
				continue;
			if ((poll_set[i].revents & POLLNVAL) && (poll_set[i].revents & POLLERR))
			{
				log "ERROR" line;
				exit(1);
			}
			if (poll_set[i].revents & POLLIN)
			{
				if (poll_set[i].fd == sv.get_fd())
				{
					poll_set.push_back((struct pollfd){sv.connect(), POLLIN, 0});
				}
				else
				{
					recv(poll_set[i].fd, buffer, sizeof(buffer), 0);
					if (buffer[0])
						request.parseRequest(buffer);
					poll_set[i].events = POLLOUT | POLLIN;
					if (poll_set[i].revents & POLLHUP)
					{
						log "end request" line;
						request.clear();
						close(poll_set[i].fd);
						poll_set.erase(poll_set.begin() + i);
					}
					// log "----request: " << buffer line;
				}
			}
			if (poll_set[i].revents & POLLOUT && request.isDone)
			{
				printf("fd => %d %d %d\n", poll_set[i].fd, poll_set[i].events, poll_set[i].revents);
				response.setRequest(request);
				response.makeResponse();
				std::string body = response.getResponse();
				fcntl(poll_set[i].fd, F_SETFL, O_NONBLOCK);
				send(poll_set[i].fd, (void *)body.c_str(), body.length(), 0); 
				poll_set[i].events = POLLIN;
			}
		}
	}
	return 0;
}