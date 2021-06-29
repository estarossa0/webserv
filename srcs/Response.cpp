#include "Webserv.hpp"

Response::Response(Connection *connection) : _connection(connection)
{
}

Response::Response(Response const &rhs)
{
	*this = rhs;
}

Response::~Response()
{
}

Response& Response::operator=(Response const &other)
{
	this->_status = other._status;
	this->_name = other._name;
	this->_ctype = other._ctype;
	this->_clen = other._clen;
	this->_body = other._body;
	this->_resp = other._resp;
	this->_cgi = other._cgi;
	this->_request = other._request;
	this->_connection = other._connection;
	this->_location = other._location;
	this->_location = other._location;
	this->_data = other._data;
	this->_servers = other._servers;
	return *this;
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

const char *Response::BadRequest::what() const throw()
{
	return "BadRequest";
}

void	Response::clear()
{
	this->_body.clear();
	this->_resp.clear();
	this->_status = 0;
	this->_ctype.clear();
	this->_cgi.clear();
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
	if (opendir((getPulicDirectory() + s).c_str()) == NULL) {
		return 0;
    }
	return 1;
}

std::string Response::getFileNameFromUri(std::string uri)
{
	std::string path;

	path = uri;
	if (isDirectory(path))
	{
		if (!this->_location.getAutoIndex() && isPreffix(_location.getPath(), path))
		{
			if (checkFileExists(path))
			{
				if (path.back() != '/')
					path.append("/"); 
				path.append(_location.getDefaultFile());
				_request.setUri(path);
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
		return "Not Allowed\r\n";
	else if (this->_status == ST_SERVER_ERROR)
		return "Internal Server Error\r\n";
	else if (this->_status == ST_NOT_IMPLEMENTED)
		return "Not Implemented\r\n";
	else if (this->_status == ST_PAYLOAD_LARGE)
		return "Payload Too Large\r\n";
	return "";
}

void Response::checkFilePermission(std::string const &path, int mode)
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
	} catch (std::exception &e)
	{
		outputLogs("exception at uploadFile: " + std::string(e.what()));
		_status = ST_SERVER_ERROR;
		throw Response::ServerError();
	}
}

std::string Response::getUploadDirectory()
{
	std::string dir = getPulicDirectory();
	dir.append(_location.getUploadLocation());
	if (dir.back() != '/')
		dir.append("/");
	return dir;
}

std::string Response::getFilePath(std::string uri)
{
	std::string dir = getPulicDirectory();
	if (uri.front() != '/')
		uri = "/" + uri;
	dir.append(uri);
	return dir;
}

std::string Response::getServerDirectory()
{
	char buffer[2000];
	std::string dir("");

	if (!getcwd(buffer, sizeof(buffer)))
	{
		_status = ST_NOT_FOUND;
		throw Response::NotFound();
	}
	else
		dir = buffer;
	return dir;
}

std::string Response::getPulicDirectory()
{
	std::string dir("");

	dir = _data.getRootDir();
	if (_location.getRootDir().length())
		dir.append(_location.getRootDir());
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
	listing.append("</pre><hr />");
	_body.replace(_body.find("$1"), 2, listing);
}

void Response::methodGet()
{
	std::string file = getFilePath(getFileNameFromUri(_request.getUri()));
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
	std::string file = getFilePath(getFileNameFromUri(_request.getUri()));
	if (_location.getUploadEnabled() && isDirectory(_request.getUri()))
		uploadFile();
	else
		readFile(file);
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
	_data = _servers[0];
	_location.setPath("");
	bool cgi = false;
	for(int i = 0; i < _servers.size(); i++)
	{
		std::vector<Location> locations = _servers[i].getLocations();
		if (this->_location.getPath().length() == 0)
			setLocation(locations[0]);
		for(std::vector<Location>::iterator it = locations.begin(); it != locations.end(); ++it)
		{
			if ((*it).isCGI() && isSuffix((*it).getPath(), _request.getUri()))
			{
				setLocation(*it);
				_data = _servers[i];
				cgi = true;
				break ;
			}
			else if (isPreffix((*it).getPath(), _request.getUri()))
			{
				if (!getLocation().getPath().length())
				{
					setLocation(*it);
					_data = _servers[i];
				}
				else if (_location.getPath().length() < (*it).getPath().length())
				{
					setLocation(*it);
					_data = _servers[i];
				}	
			}
		}
		if (cgi)
			break ;
	}
	try
	{
		if (_request.getRequestError())
		{
			_status = ST_BAD_REQUEST;
			throw Response::BadRequest();
		}
		if (_request.getContentLen() > this->_data.getClientBodySize() * 1024 * 1024)
		{
			_status = ST_PAYLOAD_LARGE;
			throw Response::PayloadLarge();
		}
		std::map<std::string, bool> loc_methods = _location.getAllowedMethods();
		if (!loc_methods[_request.getMethod()])
		{
			_status = ST_METHOD_NOT_ALLOWED;
			throw Response::MethodNotAllowed();
		}
		if (_location.isCGI())
		{
			getFileNameFromUri(_request.getUri());
			checkFilePermission(_location.getFastCgiPass(), X_OK);
			FILE *f = callCGI(_request, _data.getRootDir(), _location.getFastCgiPass());
			this->_cgi = parseCgiResponse(f);
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
	catch (std::exception &e) {
		outputLogs("exception at makeBody: " + std::string(e.what()));
	}
}

std::string Response::getDefaultErrorPage(int status)
{
	std::string default_page("	<!DOCTYPE html>\n\
	<html lang=\"en\">\n\
		<head>\n\
			<title>Http dial 3bar</title>\n\
		</head>\n\
		<body>\n\
			<h1>Error page - $1</h1>\n\
		</body>\n\
	</html>");
	default_page = default_page.replace(default_page.find("$1"), 2, std::to_string(status));
	return default_page;
}

void Response::setErrorPage()
{
	std::map<int, std::string> errors = _data.getErrorPageMap();
	
	try {
		if (errors[_status].length())
			readFile(getFilePath(errors[_status]));
		else
			throw Response::NotFound();
	} catch (std::exception &e) {
		_body = getDefaultErrorPage(_status);
	}
}

std::string Response::getResponseContentType()
{
	if (_ctype.length())
		return _ctype;
	if (isSuffix( ".html",_request.getUri()) || _location.getAutoIndex() || _status != ST_OK)
		return "text/html; charset=UTF-8";
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

std::string Response::parseCgiResponse(FILE *file)
{
	std::string buffer;
	int fd = fileno(file);
	char lines[1024];
	int r;

	while ((r = read(fd, lines, sizeof(lines))) > 0)
	{
		lines[r] = '\0';
		buffer.append(lines);
	}
	int i = buffer.find("Status: ");
	if (i != std::string::npos) {
		_status = std::stoi(buffer.substr(i + 8, 3));
		buffer.erase(i, 13);
	}
	i = buffer.find("\r\n\r\n");
    if (i != std::string::npos) {
        std::string tmp = buffer.substr(i);
        std::string len_str("Content-Length: ");
        len_str.append(std::to_string(tmp.length() - 4));
        tmp.insert(0, len_str);
        buffer.erase(i);
        buffer.append("\r\n");
        buffer.append(tmp);
    }
	if (!_status)
		_status = ST_OK;
	return buffer;
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
	if (_status == ST_MOVED_PERM && _cgi.empty())
	{
		_resp.append("Location: ");
		_resp.append(_location.getReturnUrl());
		_resp.append("\r\n\r\n");
	}
	else
	{
		_resp.append("Server: Dial3bar\r\n");
		if (_cgi.length())
			_resp.append(_cgi);
		else {
			_resp.append("Content-Type: ");
			_resp.append(getResponseContentType());
			_resp.append("\r\n");
			_resp.append("Content-Length: ");
			_resp.append(std::to_string(strlen(_body.c_str())));
			_resp.append("\r\n");
			_resp.append("\r\n");
			_resp.append(_body);
		}
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
	this->_servers = getServerData(_request.getHost());
}

Connection	*Response::getConnection()
{
	return this->_connection;
}

std::vector<ServerData> Response::getServerData(std::string name)
{
	return this->_connection->getServer()->getData(name);
}

Location Response::getLocation() const
{
	return this->_location;
}

void Response::setLocation(Location &location)
{
	this->_location = location;
}
