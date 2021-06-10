#include "websrv.h"

Request::Request() : isDone(false), clen(0), boundary(""), isArg(false)
{
}

Request::~Request()
{
}

int Request::getSpaceIndex(const std::string &str, int nbr)
{
	int i = 0;

	while (nbr)
	{
		if (str[i] == ' ')
			nbr--;
		i++;
	}
	return i;
}

bool isSuffix(std::string s1, std::string s2)
{
	int n1 = s1.length(), n2 = s2.length();
	if (n1 > n2 || !s1.length() || !s2.length())
		return false;
	for (int i = 0; i < n1; i++)
		if (s1[n1 - i - 1] != s2[n2 - i - 1])
			return false;
	return true;
}

bool isPreffix(std::string s1, std::string s2)
{
	int n1 = s1.length(), n2 = s2.length();
	if (!s1.length() || !s2.length())
		return false;
	for (int i = 0; i < n1; i++)
		if (s1[i] != s2[i])
		{
			if (i + 1 == n1)
				return true;
			return false;
		}
	return true;
}

Request::Argument Request::parseArgument(const std::string &content)
{
	std::string buffer;
	std::istringstream lines(content);
	Argument arg = {};

	while (std::getline(lines, buffer))
	{
		if (buffer.find("Content-Disposition") != std::string::npos)
			arg.disp = buffer.substr(buffer.find(":") + 2, buffer.length() - 22);
		else if (buffer.find("Content-Type") != std::string::npos)
			arg.ctype = buffer.substr(buffer.find(":") + 2, buffer.length() - 15);
		else if (buffer.length() && buffer[0] != '\r')
		{
			if (arg.data.length())
				arg.data.append("\n");
			arg.data = arg.data.append(buffer);
		}
	}
	return arg;
}

void Request::appendToBody(const std::string &content)
{
	// log "content: " << content line;
	// log boundary.length() line;
	// log isSuffix(boundary, content) line;
	// log isPreffix(boundary, content) line;
	if (boundary.length() && isSuffix(boundary, content))
	{
		log "New Argument: " << body line;
		if (!isArg)
			isArg = true;
		else
		{
			body.pop_back();
			args.push_back(parseArgument(body));
			body = "";
		}
	}
	else if (isPreffix(boundary, content))
	{
		log "ended" line;
		isArg = false;
		body.pop_back();
		args.push_back(parseArgument(body));
		body = "";
		isDone = true;
	}
	else if (isArg)
		body.append(content).append("\n");
}

void Request::parseRequest(const std::string &data)
{
	int status = 0;
	std::string buffer;
	std::istringstream lines(data);

	log data line;
	isDone = false;
	while (std::getline(lines, buffer))
	{
		log "current line: " << buffer line;
		if (!clen)
		{
			if (!method.length() && buffer.find("HTTP/1.1") != std::string::npos)
			{
				method = buffer.substr(0, getSpaceIndex(buffer, 1) - 1);
				uri = buffer.substr(getSpaceIndex(buffer, 1), getSpaceIndex(buffer, 2) - getSpaceIndex(buffer, 1) - 1);
				status = 1;
				isDone = false;
			}
			else if (buffer.find("Content-Type") != std::string::npos)
			{
				if (!boundary.length())
				{
					ctype = buffer.substr(buffer.find_first_of(": ") + 2, buffer.find_first_of(";") - 14);
					boundary = boundary.append("--").append(buffer.substr(buffer.find("boundary=") + 9));
				}
				else
				{
					ftype = buffer.substr(buffer.find(":") + 2);
				}
			}
			else if (buffer.find("Content-Disposition") != std::string::npos)
			{
				disp = buffer.substr(buffer.find(":") + 2);
			}
			else if (buffer.find("Content-Length") != std::string::npos)
			{
				clen = std::stoi(buffer.substr(buffer.find(":") + 2));
			}
			else if (buffer.find("Connection") != std::string::npos)
			{
				connection = buffer.substr(buffer.find(":") + 2);
			}
			else
				appendToBody(buffer);
			// else if (buffer.find("&") != std::string::npos)
			// {
			// 	// add to args if no connection is added
			// }
		}
		else
			appendToBody(buffer);
	}
	log "Arguments: " << args.size() line;
	for (size_t i = 0; i < args.size(); i++)
	{
		log args[i].data line;
	}
}

void Request::clear()
{
	log "Clearing request" line;
	this->method.clear();
	this->uri.clear();
	this->body.clear();
	this->ctype.clear();
	this->ftype.clear();
	this->disp.clear();
	this->boundary.clear();
	this->args.clear();
	this->connection.clear();
	this->clen = 0;
	isDone = false;
	isArg = false;
}

const std::string &Request::getMethod() const
{
	return this->method;
}

const std::string &Request::getUri() const
{
	return this->uri;
}

unsigned int Request::getContentLen() const
{
	return this->clen;
}

const std::string &Request::getContentType() const
{
	return this->ctype;
}

const std::string &Request::getBody() const
{
	return this->body;
}

size_t Request::getLenArguments()
{
	return this->args.size();
}

Request::Argument Request::getArgument(int i)
{
	return this->args[i];
}