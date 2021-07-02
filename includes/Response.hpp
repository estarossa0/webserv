#pragma once

#include "Webserv.hpp"
#include "Request.hpp"
class Request;
class Connection;

FILE* callCGI(Request &, std::string const &, std::string const &);

class Response
{
private:
	size_t _status;
	std::string _name;
	std::string _ctype;
	int _clen;
	std::string _body;
	std::string _resp;
	std::string _cgi;
	Request _request;
	Connection *_connection;
	Location _location;
	ServerData _data;
	std::vector<ServerData> _servers;

	std::string getCodeStatus();

	void methodGet();
	void methodPost();
	void methodDelete();
	void httpRedirection();
	void generateDirectoryListing();

	void checkFilePermission(std::string const &, int);
	void deleteFile(std::string &);
	void readFile(std::string);
	void uploadFile();
	void setErrorPage();
	bool isDirectory(const std::string &, int);
	std::string getDefaultErrorPage(int);
	std::string getResponseContentType();

	std::string getUploadDirectory();
	std::string getFilePath(std::string);
	std::string getPulicDirectory();
	std::string getServerDirectory();
	std::string getFileNameFromUri(std::string);
	bool		checkFileExists(std::string &);
	std::string getFileNameFromDisp(std::string);
	void makeBody();
	std::string parseCgiResponse(FILE *);
public:
	Response(Connection *);
	Response(Response const &);
	~Response();

	Response& operator=(Response const &);

	void makeResponse();
	void setRequest(Request);
	void clear();

	unsigned int getStatus() const;
	const std::string &getName() const;
	const std::string &getContentType() const;
	int getContentLength() const;
	const std::string &getBody() const;
	const std::string &getResponse() const;
	int getResponseLength() const;

	Connection *getConnection();
	std::vector<ServerData> getServerData(std::string);
	Location getLocation() const;

	void	updateContentLength(int);
	void	setLocation(Location &);

	class NotFound : public std::exception
	{
		virtual const char *what() const throw();
	};

	class NotImplemented : public std::exception
	{
		virtual const char *what() const throw();
	};

	class MethodNotAllowed : public std::exception
	{
		virtual const char *what() const throw();
	};

	class Forbidden : public std::exception
	{
		virtual const char *what() const throw();
	};

	class ServerError : public std::exception
	{
		virtual const char *what() const throw();
	};

	class PayloadLarge : public std::exception
	{
		virtual const char *what() const throw();
	};

	class BadRequest : public std::exception
	{
		virtual const char *what() const throw();
	};

	class BadGateway : public std::exception
	{
		virtual const char *what() const throw();
	};
};
