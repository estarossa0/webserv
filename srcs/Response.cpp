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

const char *Response::PayloadLarge::what() const throw()
{
	return "PayloadTooLarge";
}

void	Response::clear()
{
	// this->_request.clear();
	this->_resp.clear();
}

std::string Response::getFileNameFromDisp(std::string disp)
{
	return disp.substr(disp.find("filename=\"") + 10, disp.length() - disp.find("filename=\"") - 11);
}

bool	Response::checkFileExists(std::string &path)
{
	return true;
}

bool Response::isDirectory(const std::string &s)
{
	if (opendir((getCurrentDirectory() + s).c_str()) == NULL) {
		return 0;
    }
	return 1;
}

std::string Response::getFileNameFromUri(std::string uri)
{
	std::string path;
	if (uri.find("?") != std::string::npos)
		path = uri.substr(0, uri.find("?"));
	else
		path = uri;
	if (isDirectory(path))
	{
		if (isPreffix(_location.getPath(), path) && !this->_location.getAutoIndex())
		{
			for (std::vector<std::string>::iterator it = _location.getDefaultFiles().begin(); it != _location.getDefaultFiles().end(); ++it)
			{
				if (checkFileExists(path))
				{
					if (path.back() != '/')
						path.append("/"); 
					path.append(*it);
					_request.setUri(path);
					break ;
				}
			}
		}
	}
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
	{
		_status = ST_SERVER_ERROR;
		throw Response::ServerError();
	}
	// else
		// _body = "File Deleted";
}

void Response::readFile(std::string path)
{
	checkFilePermission(path, R_OK);
	std::string buffer;
	std::ifstream fileReader(path);
	std::string body;

	if (!fileReader.good())
	{
		_status = ST_NOT_FOUND;
		throw Response::NotFound();
	}
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
		// _body = "File Uploaded";
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
	if (dir.back() != '/')
		dir.append("/");
	return dir;
}

std::string Response::getFilePath(std::string uri)
{
	std::string dir = getCurrentDirectory();
	if (uri.front() != '/')
		uri = "/" + uri;
	dir.append(uri);
	return dir;
}

//must reset error to be thrown
std::string Response::getCurrentDirectory()
{
	char buffer[2000];
	std::string dir("");

	if (!getcwd(buffer, sizeof(buffer)))
	{
		_status = ST_NOT_FOUND;
		throw Response::NotFound();
	}
	else
	{
		dir = buffer;
		if (_location.getRootDir().length())
			dir.append(_location.getRootDir());
		else
			dir.append(getServerData().getRootDir());
		return dir;
	}
	return dir;
}

std::string getHtmlSkeleton()
{
	return "<!DOCTYPE html>\
	<html lang=\"en\">\
	<head>\
		<meta charset=\"UTF-8\">\
		<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\
		<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
		<title>Http dial 3bar</title>\
	</head>\
	<body>\
		$1\
	</body>\
	</html>";
}

void Response::generateDirectoryListing()
{
	DIR *dir;
	struct dirent *dp;
	std::string name;
	std::string listing;
	std::string path;

	_body.append(getHtmlSkeleton());
	listing.append("<h1>Index of: " + _request.getUri() + "</h1>\n<hr />\n<pre>");
	dir = opendir(getFilePath(_request.getUri()).c_str());
	while ((dp = readdir (dir)) != NULL) {
		name = dp->d_name;
		if (dp->d_type == DT_DIR)
			name.append("/");
		listing.append("<p><a href=\""+ name+"\">"+ name +"</a></p>");
	}
	listing.append("</pre>");
	_body.replace(_body.find("$1"), 2, listing);
}

void Response::methodGet()
{
	std::string file = getFilePath(getFileNameFromUri(_request.getUri()));
	// log isDirectory(_request.getUri()) line;
	if (isDirectory(_request.getUri()))
	{
		if (_location.getAutoIndex())
			generateDirectoryListing();
		else
		{
			_status = ST_NOT_FOUND;
			throw Response::NotFound();
		}
	}
	else
		readFile(file);
	_status = ST_OK;
}

void Response::methodPost()
{
	if (_location.getUploadEnabled())
		uploadFile();
	_status = ST_OK;
}

void Response::methodDelete()
{
	std::string file = getFilePath(getFileNameFromUri(_request.getUri()));
	if (isDirectory(file))
	{
		_status = ST_NOT_FOUND;
		throw Response::NotFound();
	}
	deleteFile(file);
	_status = ST_OK;
}

void Response::httpRedirection()
{
	_status = _location.getReturnCode();
}

void Response::makeBody()
{
	std::vector<Location> locations = getServerData().getLocations();
	setLocation(locations[0]);
	for(std::vector<Location>::iterator it = locations.begin(); it != locations.end(); ++it)
	{
		if (isPreffix((*it).getPath(), _request.getUri()))
		{
			if (!getLocation().getPath().length())
				setLocation(*it);
			else if (_location.getPath().length() < (*it).getPath().length())
				setLocation(*it);
		}
	}
	try
	{
		if (_request.getContentLen() > this->getServerData().getClientBodySize() * 1024 * 1024)
		{
			// throw MAX_BODY_SIZE_ERROR;
			_status = ST_PAYLOAD_LARGE;
			throw Response::PayloadLarge();
		}
		std::map<std::string, bool> loc_methods = _location.getAllowedMethods();
		if (!loc_methods[_request.getMethod()])
		{
			_status = ST_NOT_IMPLEMENTED;
			throw Response::NotImplemented();
		}
		if (_location.isCGI())
		{
			// parse request headers
			// go to cgi
		}
		else if (_location.isRedirection())
			httpRedirection();
		else if (_request.getMethod().compare("GET") == 0)
			methodGet();
		else if (_request.getMethod().compare("POST") == 0)
			methodPost();
		else if (_request.getMethod().compare("DELETE") == 0)
			methodDelete();
	}
	catch (std::exception &e)
	{
		log "Exception at makeBody : " << e.what() line;
	}
}

std::string Response::getDefaultErrorPage(int status)
{
	std::string default_page("<!DOCTYPE html>\n\
		<html lang=\"en\">\n\
		<head>\n\
			<meta charset=\"UTF-8\">\n\
			<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n\
			<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
			<title>Http dial 3bar</title>\n\
		</head>\n\
		<body>\n\
			<h1>Not Found - $1</h1>\n\
		</body>\n\
		</html>");
	default_page = default_page.replace(default_page.find("$1"), 2, std::to_string(status));
	return default_page;
}

void Response::setErrorPage()
{
	std::map<int, std::string> errors = getServerData().getErrorPageMap();
	
	if (errors[_status].length())
		readFile(getFilePath(errors[_status]));
	else
		_body = getDefaultErrorPage(_status);
}

std::string Response::getResponseContentType()
{
	if (isSuffix( ".html",_request.getUri()) || _location.getAutoIndex())
		return "text/html";
	else if (isSuffix( ".css",_request.getUri()))
		return "text/css";
	else if (isSuffix( ".json",_request.getUri()))
		return "application/json";
	else if (isSuffix( ".xml",_request.getUri()))
		return "application/xml";
	else if (isSuffix( ".js",_request.getUri()))
		return "aplication/javascript";
	else
		return "text/plain";
}

void Response::makeResponse()
{
	makeBody();
	if (_status != ST_OK)
		setErrorPage();
	_resp = "HTTP/1.1 ";
	_resp.append(std::to_string(_status));
	_resp.append(" ");
	_resp.append(getCodeStatus());
	if (_status == ST_MOVED_PERM)
	{
		_resp.append("Location: ");
		_resp.append(_location.getReturnUrl());
		_resp.append("\r\n\r\n");
	}
	else
	{
		_resp.append("Server: Dial3bar\r\n");
		_resp.append("Content-Type: ");
		_resp.append(getResponseContentType());
		_resp.append("\r\n");
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
