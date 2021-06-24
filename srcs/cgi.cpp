#include "websrv.h"

size_t		envSize()
{
	size_t index;
	index = 0;

	while (environ && environ[index])
	{
		index++;
	}
	return (index);
}

void		copyEnv(std::vector<std::string> &array, char** env)
{
	size_t		index = 0;

	while (environ[index])
	{
		env[index] = environ[index];
		index++;
	}
	for (size_t i = 0; i < array.size(); i++)
	{
		env[index] = strdup(array[i].c_str());
		index++;
	}
	env[index] = NULL;
}

FILE*	callCGI(Request &req)
{
	std::FILE*					tmpf;
	int							pipefds[2];
	std::vector<std::string>	array;
	static size_t				size = envSize();
	char**						env;
	char**						argv;
	int							fd;
	pid_t						pid;

	tmpf = std::tmpfile();

	argv = (char**) malloc(sizeof(char*) * 3);
	argv[0] = strdup("php-cgi");
	argv[1] = strdup("index.php");
	argv[2] = NULL;
	// CONT TYPE
	// CONT LENGHT
	// GATE INTERFACE
	// SERVER PROTOCOL
	// SERVER PORT
	// REQUEST METHOD
	// SERVER NAME
	// SERVER SOFTWARE
	// REMOTE ADDR
	// PATH INFO
	// PATH TRANSLATED
	// SCRIPT NAME

	// QUERY STRING
	// HTTP_*

	array.push_back("CONTENT_LENGTH=" + std::to_string(req.getContentLen()));
	array.push_back("CONTENT_TYPE=" + req.getContentType());
	array.push_back("GATEWAY_INTERFACE=CGI/1.1");
	array.push_back("SERVER_PROTOCOL=HTTP/1.1");
	array.push_back("SERVER_PORT=" +  std::to_string(req.getConnection()->getPort()));
	array.push_back("REQUEST_METHOD=" + req.getMethod());
	array.push_back("SERVER_NAME=SVNAME" ); // + req.getConnection()->getServer()->_data.getHost()
	array.push_back("SERVER_SOFTWARE=webserv");
	array.push_back("REMOTE_ADDR=" + req.getConnection()->getIp());
	array.push_back("PATH_INFO=" + req.getUri());
	array.push_back("PATH_TRANSLATED=" + std::string(getcwd(NULL,0)) + req.getUri());
	array.push_back("SCRIPT_NAME=" + req.getUri());

	pid = fork();

	if (pid == 0)
	{
		fd = fileno(tmpf);
		env = (char **)malloc((size + array.size() + 1) * sizeof(char *));
		copyEnv(array, env);
		dup2(fd, STDOUT_FILENO);

		chdir("/Users/arraji/Desktop/webserv"); //WILL BE CHANGED TO ROOT PATH
		execve("/Users/arraji/Desktop/webserv/php-cgi", argv, env); //WILL BE CHANGED TO CGI PARSED PATH
	}
	else
		waitpid(pid, nullptr, 0);
	std::rewind(tmpf);
	return tmpf;
}
