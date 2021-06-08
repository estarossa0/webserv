#include "websrv.h"

std::string getCurrentDirectory()
{
	char buffer[2000];

	if (!getcwd(buffer, sizeof(buffer)))
		perror("getcwd() error");
	else
	{
		std::string dir(buffer);
		return dir;
	}
	return "";
}

std::string getFilePath(std::string uri)
{
	std::string dir = getCurrentDirectory();
	dir.append("/public");
	dir.append(uri);
	return dir;
}

std::string readFile(std::string path)
{
	// check file if exists
	// throw exception if error
	std::string buffer;
	std::ifstream fileReader(path);
	std::string body;
	while (getline(fileReader, buffer))
		body.append(buffer).append("\n");
	fileReader.close();
	return body;
}

void deleteFile(std::string path)
{
	//check file exist
	// throw exception if error
	if (std::remove(path.c_str()) != 0)
		log "Deleting file error" line;
	else
		log "File deleted successfully" line;
}

std::string methodGet(Request request)
{
	std::string file = getFilePath(request.getUri());
	return readFile(file);
}

void methodPost(Request request)
{
	// for file upload
}

void methodDelete(Request request)
{
	std::string file = getFilePath(request.getUri());
	deleteFile(file);
}

std::string manageMethods(Request request)
{
	std::string res;
	try
	{
		// log "|" << request.getMethod() << "|" line;
		if (request.getMethod().compare("GET") == 0)
			res = methodGet(request);
		else if (request.getMethod() == "POST")
			methodPost(request);
		else if (request.getMethod() == "DELETE")
			methodDelete(request);
		else
		{
			// throw method not allowed 405
		}
	}
	catch (std::exception e)
	{
		log e.what() line;
	}
	return res;
}

std::string getContentType(int isHtml)
{
	if (isHtml)
		return "text/html; charset=UTF-8\r\n";
	else
		return "text/plain\r\n";
}

std::string makeResponse(std::string body, int status, int isHtml)
{
	std::string header("HTTP/1.1 ");
	header.append(std::to_string(status));
	header.append(" OK\r\n");
	header.append("Server: Dial3bar\r\n");
	header.append("Content-Type: ");
	header.append(getContentType(isHtml));
	if (body.length() > 0)
	{
		header.append("Content-Length: ");
		header.append(std::to_string(strlen(body.c_str())));
		header.append("\r\n\n");
		header.append(body);
		header.append("\r\n");
	}
	else
		header.append("\n");
	log "response: " << header line;
	return header;
}

#define PORT 80
int main()
{
	char buffer[30000] = {0};
	char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 277\r\n";
	Server sv(PORT);
	Request request;
	std::vector<struct pollfd> poll_set;
	poll_set.push_back((struct pollfd){sv.get_fd(), POLLIN, 0});
	int p;
	while (1)
	{
		bzero(buffer, 30000);
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
						request.clear();
						close(poll_set[i].fd);
						poll_set.erase(poll_set.begin() + i);
					}
					// log "----request: " << buffer line;
				}
			}
			if (poll_set[i].revents & POLLOUT)
			{
				std::string body;
				printf("fd => %d %d %d\n", poll_set[i].fd, poll_set[i].events, poll_set[i].revents);
				// body = manageMethods("GET", "/test.txt");  // params: Method   uri_path
				// body = manageMethods("GET", "/index.html");  // params: Method   uri_path
				// body = manageMethods("POST", "/upload");  // params: Method   uri_path
				//body = manageMethods("DELETE", "/todelete");  // params: Method   uri_path
				// body = "Hello world\n\n";
				body = manageMethods(request); // params: Method   uri_path
				std::string resp = makeResponse(body, 200, 0); // params: Body  status_code   isHtml
				// send(poll_set[i].fd, (char *)response, strlen(response), 0); // response header
				fcntl(poll_set[i].fd, F_SETFL, O_NONBLOCK);
				send(poll_set[i].fd, (void *)resp.c_str(), resp.length(), 0); // response header

				// send(poll_set[i].fd, (void *)"\n", 1, 0);					 //indicate end of header
				// send(poll_set[i].fd, (void *)"hello", 5, 0);				 // body message
				// send(poll_set[i].fd, (void *)res.c_str(), strlen(res.c_str()), 0);
				// printf("Hello message sent\n");
				poll_set[i].events = POLLIN;
			}
		}
	}
	return 0;
}