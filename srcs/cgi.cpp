#include "Webserv.hpp"

static	std::string metaVarSyntax(std::string str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == '-')
			str[i] = '_';
		else
			str[i] = toupper(str[i]);
	}
	return str;
}

FILE*	callCGI(Request &req, std::string const &root, std::string const &cgi_path)
{
	std::FILE*					tmpf;
	char				const*	argv[3];
	int							fd;
    int							fds[2];
	pid_t						pid;

	tmpf = std::tmpfile();

	setenv("CONTENT_LENGTH", std::to_string(req.getContentLen()).c_str(), 1);
	if (req.getConnectionType().size())
		setenv("CONTENT_TYPE", req.getOriginContentType().c_str(), 1);
	setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
	setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
	setenv("SERVER_PORT", std::to_string(req.getConnection()->getPort()).c_str(), 1);
	setenv("REQUEST_METHOD", req.getMethod().c_str(), 1);
	setenv("SERVER_NAME", "webserv", 1);
	setenv("REDIRECT_STATUS", "1", 1);
	setenv("REMOTE_ADDR", req.getConnection()->getIp().c_str(), 1);
	setenv("PATH_INFO", req.getUri().c_str(), 1);
	setenv("PATH_TRANSLATED", (root + req.getUri()).c_str(), 1);
	setenv("QUERY_STRING", req.getQuery().c_str(), 1);

	std::vector<Request::Header> headers = req.getHeaders();
	for (std::vector<Request::Header>::iterator it = headers.begin(); it != headers.end(); ++it)
		setenv(("HTTP_" + metaVarSyntax(it->name)).c_str(), it->value.c_str(), 1);

	pipe(fds);
	pid = fork();

	if (pid == 0)
	{
		dup2(fds[0], 0);
		close(fds[1]);
		close(fds[0]);
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
	{
		close(fds[0]);
		write(fds[1], req.getBody().c_str(), req.getBody().length());
		close(fds[1]);
		waitpid(pid, nullptr, 0);
	}
	std::rewind(tmpf);
	return tmpf;
}
