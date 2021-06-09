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

std::string getFileNameFromUri(std::string uri)
{
	if (uri.find("?") != std::string::npos)
		return uri.substr(0, uri.find("?"));
	return uri;
}



std::string Response::getCodeStatus()
{
	if (this->_status == ST_OK)
		return "OK\r\n";
	else if (this->_status == ST_MOVED_PERM)
		return "Moved permenantly\r\n";
	else if (this->_status == ST_BAD_REQUEST)
		return "Bad Request\r\n";
	else if (this->_status == ST_FORBIDDEN)
		return "Forbidden\r\n";
	else if (this->_status == ST_NOT_FOUND)
		return "Not Found\r\n";
	else if (this->_status == ST_METHOD_NOT_ALLOWED)
		return "Method Not Allowed\r\n";
	else if (this->_status == ST_SERVER_ERROR)
		return "Internal Server Error\r\n";
	else if (this->_status == ST_NOT_IMPLEMENTED)
		return "Not Implemented\r\n";
	return "";
}

void Response::checkFilePermission(std::string &path, int mode)
{
	int returnval = access(path.c_str(), mode);
	if (returnval != 0)
	{
		if (errno == ENOENT)
		{
			_status = ST_NOT_FOUND;
			throw Response::NotFound();
		}
		else if (errno == EACCES)
		{
			_status = ST_FORBIDDEN;
			throw Response::Forbidden();
		}
	}
}



void Response::deleteFile(std::string &path)
{
	checkFilePermission(path, W_OK);
	_body = "File Deleted";
}

void Response::readFile(std::string &path)
{
	checkFilePermission(path, R_OK);
	std::string buffer;
	std::ifstream fileReader(path);
	std::string body;

	while (getline(fileReader, buffer))
		body.append(buffer).append("\n");
	fileReader.close();
	_body = body;
}



std::string getFileNameFromDisp(std::string disp)
{
	// log disp.substr(disp.find("filename=\"") + 10, disp.length() - disp.find("filename=\"") - 11) line;
	return disp.substr(disp.find("filename=\"") + 10, disp.length() - disp.find("filename=\"") - 11);
}

void Response::uploadFile()
{
	// check if file already exists = > should upload or not
	for (size_t i = 0; i < _request.getLenArguments(); i++)
	{
		Request::Argument arg = _request.getArgument(i);
		if (_request.getArgument(i).ctype != "")
		{
			std::string dir = getFilePath("/" + getFileNameFromDisp(arg.disp));
			std::ofstream file(dir);
			file << arg.data;
			file.close();
		}
	}
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

void Response::methodGet()
{
	//check cgi
	std::string file = getFilePath(getFileNameFromUri(_request.getUri()));
	readFile(file);
}

void Response::methodPost()
{
	//check cgi
	//check location with uri
	uploadFile();
	_body = "File Uploaded";
}

void Response::methodDelete()
{
	//check cgi
	std::string file = getFilePath(getFileNameFromUri(_request.getUri()));
	deleteFile(file);
}

void Response::makeBody()
{
	std::string res;
	try
	{
		if (_request.getMethod().compare("GET") == 0)
			methodGet();
		else if (_request.getMethod().compare("POST") == 0)
			methodPost();
		else if (_request.getMethod().compare("DELETE") == 0)
			methodDelete();
		else
		{
			_status = ST_NOT_IMPLEMENTED;
			throw Response::NotImplemented();
		}
		_status = ST_OK;
	}
	catch (std::exception &e)
	{
		log "Exception: " << e.what() line;
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
	if (_status != ST_OK)
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
		_resp.append("\n\r\n");
	log "response: " << _resp line;
}

const std::string &Response::getResponse() const
{
	return this->_resp;
}

void Response::setRequest(Request request)
{
	this->_request = request;
}