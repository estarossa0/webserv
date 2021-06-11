#pragma once

#include "websrv.h"

class Request;

class Response
{
private:
	unsigned int _status;
	std::string _name;
	std::string _ctype;
	std::string _clen;
	std::string _body;
	std::string _resp;
	Request _request;
	Connection *_connection;
	// Location _location;

	std::string getCodeStatus();

	void methodGet();
	void methodPost();
	void methodDelete();

	void checkFilePermission(std::string &, int);
	void deleteFile(std::string &);
	void readFile(std::string &);
	void uploadFile();

	std::string getUploadDirectory();
	std::string getFilePath(std::string);
	std::string getCurrentDirectory();
	void makeBody();

public:
	Response(Connection *);
	~Response();

	void makeResponse();
	void setRequest(Request);
	void clear();

	unsigned int getStatus() const;
	const std::string &getName() const;
	const std::string &getContentType() const;
	const std::string &getContentLength() const;
	const std::string &getBody() const;
	const std::string &getResponse() const;
	Connection *getConnection();
	// Data *getServerData();
	// Location getLocation() const;

	// void	setLocation(Location &);

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
};