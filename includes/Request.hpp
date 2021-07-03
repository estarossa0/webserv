#pragma once

#include "Webserv.hpp"

class Connection;

class Request
{
public:
	struct Argument
	{
		std::string disp;
		std::string ctype;
		std::string data;
	};

	struct Header
	{
		std::string name;
		std::string value;
	};

private:
	std::string _data;
	std::string _method;
	std::string _uri;
	std::string _query;
	std::string _host;
	std::string _protocol;
	unsigned int _clen;
	std::string _ctype;
	std::string _ctype_origin;
	std::string _body;
	std::string _boundary;
	std::string _disp;
	std::string _contype;
	std::vector<Header> _headers;
	std::vector<Argument> _args;
	Connection *_connection;
	bool _isArg;
	int _error;

	void appendToBody(std::string );
	int getSpaceIndex(const std::string &, int);
	Argument parseArgument(const std::string &);
	bool validateContentLength();

public:
	bool isDone;

	Request();
	Request(Connection *);
	~Request();

	void parseRequest();
	void parseBody();
	void clear();
	void printRequest();
	void parseHeader(std::string &);

	bool checkDataDone();

	const std::string &getMethod() const;
	const std::string &getUri() const;
	const std::string &getQuery() const;
	const std::string &getProtocol() const;
	const std::string &getHost() const;
	int getContentLen() const;
	const std::string &getContentType() const;
	const std::string &getOriginContentType() const;
	const std::string &getBody() const;
	const std::string &getBoundary() const;
	const std::string &getDisposition() const;
	const std::string &getData() const;
	const std::string &getConnectionType() const;
	void setConnectionType(std::string const &);
	Connection	*getConnection();
	std::vector<ServerData> getServerData(std::string &);
	int getRequestError();

	void setUri(std::string const &);

	void	appendToData(char *);

	size_t getLenArguments();
	Argument getArgument(int i);
	std::vector<Argument> getArguments();
	std::vector<Header> getHeaders();
};

bool isPreffix(std::string, std::string);
bool isSuffix(std::string, std::string);
