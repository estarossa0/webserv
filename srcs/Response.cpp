#include "websrv.h"

Response::Response()
{
}

Response::Response(Request request) : _request(request)
{
}

Response::~Response()
{
}

const char *Response::NotFound::what() const throw()
{
	return "Not Found";
}

const char *Response::NotImplemented::what() const throw()
{
	return "NotImplemented";
}

const char *Response::MethodNotAllowed::what() const throw()
{
	return "MethodNotAllowed";
}

const char *Response::Forbidden::what() const throw()
{
	return "Forbidden";
}


std::string Response::getCodeStatus()
{
	if (this->_status == 200)
		return "OK\r\n";
	else if (this->_status == 301)
		return "Moved permenantly\r\n";
	else if (this->_status == 400)
		return "Bad Request\r\n";
	else if (this->_status == 403)
		return "Forbidden\r\n";
	else if (this->_status == 404)
		return "Not Found\r\n";
	else if (this->_status == 405)
		return "Method Not Allowed\r\n";
	else if (this->_status == 500)
		return "Internal Server Error\r\n";
	else if (this->_status == 501)
		return "Not Implemented\r\n";
	return "";
}

void Response::methodGet()
{
	std::string file = getFilePath(_request.getUri());
	readFile(file);
}

void Response::methodPost()
{
	_status = 200;
}

void Response::methodDelete()
{
	std::string file = getFilePath(_request.getUri());
	deleteFile(file);
}

void Response::deleteFile(std::string &path)
{
	if (access(path.c_str(), F_OK) == -1)
	{
		_status = 404;
		throw Response::NotFound();
	}
	if (std::remove(path.c_str()) != 0)
	{
		_status = 403;
		throw Response::Forbidden();
	}
	_status = 200;
}

void Response::readFile(std::string &path)
{
	if (access(path.c_str(), F_OK) == -1)
	{
		_status = 404;
		throw Response::NotFound();
	}
	// throw permission 403 error
	std::string buffer;
	std::ifstream fileReader(path);
	std::string body;
	while (getline(fileReader, buffer))
		body.append(buffer).append("\n");
	fileReader.close();
	_body = body;
	_status = 200;
}

void Response::uploadFile()
{
}

std::string Response::getFilePath(std::string uri)
{
	std::string dir = getCurrentDirectory();
	dir.append("/public");
	dir.append(uri);
	return dir;
}

std::string Response::getCurrentDirectory()
{
	char buffer[2000];
	std::string dir("");

	if (!getcwd(buffer, sizeof(buffer)))
		perror("getcwd() error");
	else
	{
		dir = buffer;
		return dir;
	}
	return dir;
}

void Response::makeBody()
{
	std::string res;
	try
	{
		if (_request.getMethod().compare("GET") == 0)
			methodGet();
		else if (_request.getMethod() == "POST")
			methodPost();
		else if (_request.getMethod() == "DELETE")
			methodDelete();
		else
		{
			_status = 405;
			throw Response::NotImplemented();
		}
	}
	catch (std::exception &e)
	{
		log "exception: " << e.what() line;
	}
}

void Response::makeResponse()
{
	makeBody();
	_resp = "HTTP/1.1 ";
	_resp.append(std::to_string(_status));
	_resp.append(" ");
	_resp.append(getCodeStatus());
	// set server name
	_resp.append("Server: Dial3bar\r\n");
	// _resp.append(getContentType(isHtml));
	if (_status != 200)
	{
		_resp.append("Content-Type: ");
		_resp.append("text/plain\r\n");
		_resp.append("Content-Length: 10\r\n\n");
		// read error page
		_resp.append("Error page\r\n");
	}
	if (_body.length() > 0)
	{
		_resp.append("Content-Type: ");
		// get content type 
		_resp.append("text/plain\r\n");
		_resp.append("Content-Length: ");
		_resp.append(std::to_string(strlen(_body.c_str())));
		_resp.append("\r\n\n");
		_resp.append(_body);
		_resp.append("\r\n");
	}
	else
		_resp.append("");
	// log "response: " << _resp line;
}

const std::string &Response::getResponse() const
{
	return this->_resp;
}

void Response::setRequest(Request request)
{
	this->_request = request;
}