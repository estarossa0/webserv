#pragma once

#include "websrv.h"

class Request
{
private:
	struct Argument
	{
		std::string disp;
		std::string ctype;
		std::string data;
	};

	std::string method;
	std::string uri;
	unsigned int clen;
	std::string ctype; // content-type
	std::string ftype; // form-type
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

	void parseRequest(const std::string &);
	void clear();

	const std::string &getMethod() const;
	const std::string &getUri() const;
	unsigned int getContentLen() const;
	const std::string &getContentType() const;
	const std::string &getBody() const;
	const std::string &getBoundary() const;
	const std::string &getDisposition() const;
};
