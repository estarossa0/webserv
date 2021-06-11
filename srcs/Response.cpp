#include "websrv.h"

Response::Response(Connection *connection) : _connection(connection)
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

const char *Response::ServerError::what() const throw()
{
	return "ServerError";
}

void	Response::clear()
{
	// this->_request.clear();
	this->_resp.clear();
}

std::string Response::getFileNameFromDisp(std::string disp)
{
	// log disp.substr(disp.find("filename=\"") + 10, disp.length() - disp.find("filename=\"") - 11) line;
	return disp.substr(disp.find("filename=\"") + 10, disp.length() - disp.find("filename=\"") - 11);
}

bool	Response::checkFileExists(std::string &path)
{
	return true;
}

std::string Response::getFileNameFromUri(std::string uri)
{
	std::string path;
	if (uri.find("?") != std::string::npos)
		path = uri.substr(0, uri.find("?"));
	else
		path = uri;
	if (path.compare(this->_location.getPath()) == 0 && this->_location.getAutoIndex())
	{
		for (std::vector<std::string>::iterator it = _location.getDefaultFiles().begin(); it != _location.getDefaultFiles().end(); ++it)
		{
			if (checkFileExists(*it))
			{
				// check if location has / at the end
				path.append("/"); 
				path.append(*it);
				break ;
			}
		}
	}
	// log "uri: " << path line;
	return path;
}

std::string Response::getCodeStatus()
{
	if (this->_status == ST_OK)
		return "OK\r\n";
	else if (this->_status == ST_MOVED_PERM)
		return "Moved Permenantly\r\n";
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
	if (std::remove(path.c_str()) != 0)
		throw Response::ServerError();
	else
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

void Response::uploadFile()
{
	// check if file already exists = > should upload or not
	try {
		for (size_t i = 0; i < _request.getLenArguments(); i++)
		{
			Request::Argument arg = _request.getArgument(i);
			if (_request.getArgument(i).ctype.length())
			{
				std::string name = getFileNameFromDisp(arg.disp);
				std::string dir = getUploadDirectory().append(name);
				std::ofstream file(dir);
				file << arg.data;
				file.close();
			}
		}
		_body = "File Uploaded";
	} catch (std::exception &e)
	{
		log "Exception at uploadFile: " << e.what() line;
		_status = ST_SERVER_ERROR;
		throw Response::ServerError();
	}
}

std::string Response::getUploadDirectory()
{
	std::string dir = getCurrentDirectory();
	dir.append(_location.getUploadLocation());
	dir.append("/");
	return dir;
}

std::string Response::getFilePath(std::string uri)
{
	std::string dir = getCurrentDirectory();

	dir.append(uri);
	return dir;
}

//must reset error to be thrown
std::string Response::getCurrentDirectory()
{
	char buffer[2000];
	std::string dir("");

	if (!getcwd(buffer, sizeof(buffer)))
		perror("getcwd() error");
	else
	{
		dir = buffer;
		if (_location.getRootDir().length())
			dir.append(_location.getRootDir());
		else
		{
			dir.append(getServerData().getRootDir());
			dir.append("/");
		}
		return dir;
	}
	return dir;
}


void Response::methodGet()
{
	std::string file = getFilePath(getFileNameFromUri(_request.getUri()));
	readFile(file);
}

void Response::methodPost()
{
	if (_location.getUploadEnabled())
		uploadFile();
}

void Response::methodDelete()
{
	std::string file = getFilePath(getFileNameFromUri(_request.getUri()));
	deleteFile(file);
}

void Response::responseRedirection()
{
	_status = _location.getReturnCode();
}

/*

=> GET /get/user

search location
if location is prefix for uri
	if accepted method
		if cgi
			gotocgi
		else
			if dir exist
				if has file ext
					get file
				else
					if autoindex
						return index file
					else
						return error file
			else
				error file
else
	error file
*/

void Response::makeBody()
{
	std::vector<Location> locations = getServerData().getLocations();

	for(std::vector<Location>::iterator it = locations.begin(); it != locations.end(); ++it)
	{
		if (_request.getUri().find((*it).getPath()) != std::string::npos)
		{
			if (!getLocation().getPath().length())
				setLocation(*it);
			else if (_location.getPath().length() < (*it).getPath().length())
				setLocation(*it);
		}
	}
	try
	{
		if (!_location.getAllowedMethods()[_request.getMethod()])
		{
			_status = ST_NOT_IMPLEMENTED;
			throw Response::NotImplemented();
		}
		if (_location.isCGI())
		{
			// go to cgi
		}
		else if (_location.isRedirection())
			responseRedirection();
		else if (_request.getMethod().compare("GET") == 0)
			methodGet();
		else if (_request.getMethod().compare("POST") == 0)
			methodPost();
		else if (_request.getMethod().compare("DELETE") == 0)
			methodDelete();
		_status = ST_OK;
	}
	catch (std::exception &e)
	{
		log "Exception at makeBody : " << e.what() line;
	}
}

std::string Response::getErrorPage()
{
	return "";
}

void Response::makeResponse()
{
	makeBody();
	_resp = "HTTP/1.1 ";
	_resp.append(std::to_string(_status));
	_resp.append(" ");
	_resp.append(getCodeStatus());
	// set server name
	if (_status == ST_MOVED_PERM)
	{
		_resp.append("Location: ");
		_resp.append(_location.getReturnUrl());
		_resp.append("\r\n\r\n");
	}
	if (_status != ST_OK)
	{
		_resp.append("Server: Dial3bar\r\n");
		_resp.append("Content-Type: ");
		_resp.append("text/plain\r\n");
		_resp.append("Content-Length: 11\r\n\n");
		// read error page
		// _resp.append
		_resp.append("\r\n\r\n");
	}
	else
	{
		_resp.append("Server: Dial3bar\r\n");
		_resp.append("Content-Type: ");
		// get content type
		_resp.append("text/plain\r\n");
		_resp.append("Content-Length: ");
		_resp.append(std::to_string(strlen(_body.c_str())));
		_resp.append("\r\n\n");
		_resp.append(_body);
		_resp.append("\r\n");
	}
}

const std::string &Response::getResponse() const
{
	return this->_resp;
}

void Response::setRequest(Request request)
{
	this->_request = request;
}

Connection	*Response::getConnection()
{
	return this->_connection;
}

ServerData Response::getServerData()
{
	return this->_connection->getServer()->getData();
}

Location Response::getLocation() const
{
	return this->_location;
}

void Response::setLocation(Location &location)
{
	this->_location = location;
}
