#pragma once

#include "websrv.h"

class Request
{
public:
	struct Argument
	{
		std::string disp;
		std::string ctype;
		std::string data;
	};

private:
	std::string data;
	std::string method;
	std::string uri;
	unsigned int clen;
	std::string ctype;
	std::string body;
	std::string boundary;
	std::string disp;
	std::string connection;
	std::vector<Argument> args;
	bool isArg;

	void appendToBody(const std::string &);
	int getSpaceIndex(const std::string &, int);
	Argument parseArgument(const std::string &);

public:
	bool isDone;
	Request();
	~Request();

	void parseRequest();
	void clear();

	const std::string &getMethod() const;
	const std::string &getUri() const;
	unsigned int getContentLen() const;
	const std::string &getContentType() const;
	const std::string &getBody() const;
	const std::string &getBoundary() const;
	const std::string &getDisposition() const;
	const std::string &getData() const;

	void	appendToData(std::string);

	size_t getLenArguments();
	Argument getArgument(int i);
};
