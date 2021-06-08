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

void Response::methodGet()
{
	std::string file = getFilePath(_request.getUri());
	readFile(file);
	_status = 200;
}

void Response::methodPost()
{
	_status = 200;
}

void Response::methodDelete()
{
	std::string file = getFilePath(_request.getUri());
	deleteFile(file);
	_status = 200;
}

void Response::deleteFile(std::string &path)
{
	//check file exist
	// throw exception if error
	if (std::remove(path.c_str()) != 0)
		log "Deleting file error" line;
	else
		log "File deleted successfully" line;
}

void Response::readFile(std::string &path)
{
	std::string buffer;
	std::ifstream fileReader(path);
	std::string body;
	while (getline(fileReader, buffer))
		body.append(buffer).append("\n");
	fileReader.close();
	_body = body;
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
		// log "|" << request.getMethod() << "|" line;
		if (_request.getMethod().compare("GET") == 0)
			methodGet();
		else if (_request.getMethod() == "POST")
			methodPost();
		else if (_request.getMethod() == "DELETE")
			methodDelete();
		else
		{
			// throw method not allowed 405
			_status = 405;
		}
	}
	catch (std::exception e)
	{
		log e.what() line;
	}
}

void Response::makeResponse()
{
	makeBody();
	_resp = "HTTP/1.1 ";
	_resp.append(std::to_string(_status));
	// get code status for the status
	_resp.append(" OK\r\n");
	_resp.append("Server: Dial3bar\r\n");
	_resp.append("Content-Type: ");
	// _resp.append(getContentType(isHtml));
	_resp.append("text/plain\r\n");
	if (_body.length() > 0)
	{
		_resp.append("Content-Length: ");
		_resp.append(std::to_string(strlen(_body.c_str())));
		_resp.append("\r\n\n");
		_resp.append(_body);
		_resp.append("\r\n");
	}
	else
		_resp.append("\n");
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