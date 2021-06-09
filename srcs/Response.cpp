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

void Response::checkFilePermission(std::string &path, int mode)
{
	int returnval = access(path.c_str(), mode);
	if (returnval != 0)
	{
		if (errno == ENOENT)
		{
			_status = 404;
			throw Response::NotFound();
		}
		else if (errno == EACCES)
		{
			_status = 403;
			throw Response::Forbidden();
		}
	}
}

void Response::methodGet()
{
	std::string file = getFilePath(getFileNameFromUri(_request.getUri()));
	readFile(file);
}

void Response::methodPost()
{
	//check location with uri
	uploadFile();
	_body = "File Uploaded";
	_status = 200;
}

void Response::methodDelete()
{
	std::string file = getFilePath(getFileNameFromUri(_request.getUri()));
	deleteFile(file);
}

void Response::deleteFile(std::string &path)
{
	checkFilePermission(path, W_OK);
	_status = 200;
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
	_status = 200;
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
		// must get filename from
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

void Response::makeBody()
{
	std::string res;
	try
	{
		// log _request.getMethod() << "|" line;
		if (_request.getMethod().compare("GET") == 0)
			methodGet();
		else if (_request.getMethod().compare("POST") == 0)
			methodPost();
		else if (_request.getMethod().compare("DELETE") == 0)
			methodDelete();
		else
		{
			_status = 501;
			throw Response::NotImplemented();
		}
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