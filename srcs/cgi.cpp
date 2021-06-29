#include "Webserv.hpp"

FILE*	callCGI(Request &req, std::string const &root, std::string const &cgi_path)
{
	std::FILE*					tmpf;
	char				const*	argv[3];
	int							fd;
	pid_t						pid;

	tmpf = std::tmpfile();

	setenv("CONTENT_LENGTH", std::to_string(req.getContentLen()).c_str(), 1);
	if (req.getConnectionType().size())
		setenv("CONTENT_TYPE", req.getContentType().c_str(), 1);
	setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
	setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
	setenv("SERVER_PORT", std::to_string(req.getConnection()->getPort()).c_str(), 1);
	setenv("REQUEST_METHOD", req.getMethod().c_str(), 1);
	setenv("SERVER_NAME", "webserv", 1);
	setenv("REMOTE_ADDR", req.getConnection()->getIp().c_str(), 1);
	setenv("PATH_INFO", req.getUri().c_str(), 1);
	setenv("PATH_TRANSLATED", (root + req.getUri()).c_str(), 1);
	setenv("QUERY_STRING", req.getQuery().c_str(), 1);

	std::vector<Request::Header> headers = req.getHeaders();
	for (std::vector<Request::Header>::iterator it = headers.begin(); it != headers.end(); ++it)
		setenv(("HTTP_" + it->name).c_str(), it->value.c_str(), 1);

	pid = fork();

	if (pid == 0)
	{
		argv[0] = cgi_path.c_str();
		argv[1] = req.getUri().c_str() + 1;
		argv[2] = NULL;
		fd = fileno(tmpf);
		dup2(fd, STDOUT_FILENO);

		chdir(root.c_str());
		if (execve(cgi_path.c_str(), (char *const *)argv, environ) == -1)
			exit(1);
	}
	else
		waitpid(pid, nullptr, 0);
	std::rewind(tmpf);
	return tmpf;
}
