#include "websrv.h"

Request::Request()
{
}

Request::Request(Connection *_connection) : isDone(false), _clen(0), _data(""), _boundary(""), _isArg(false), _connection(_connection), _error(0)
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
	bool isBody = false;

	while (std::getline(lines, buffer))
	{
		if (buffer.find("\r") != std::string::npos)
			buffer.pop_back();
		if (buffer.find("Content-Disposition") != std::string::npos)
			arg.disp = buffer.substr(buffer.find(":") + 2);
		else if (buffer.find("Content-Type") != std::string::npos)
			arg.ctype = buffer.substr(buffer.find(":") + 2);
		else
		{
			if (isBody)
			{
				arg.data.append(buffer);
				arg.data.append("\n");
			}
			else
				isBody = true;
		}
	}
	arg.data.pop_back();
	return arg;
}

int	getPostBodyLength(std::string data)
{
	int len = 0;

	size_t i = data.find("\r\n\r\n");
	len = std::stoi(data.substr(data.find("Content-Length: ") + 16));
	std::string tmp = data.substr(i + 4);
	return tmp.length();
}

void Request::parseHeader(std::string &data)
{
	Request::Header header = {};

	header.name = data.substr(0 , data.find(":"));
	header.value = data.substr(data.find(":") + 2, data.length() - data.find(":") - 3);
	_headers.push_back(header);
}

bool isBoundary(std::string s1, std::string s2)
{
	s2.pop_back();
	if (s1.compare(s2) == 0)
		return true;
	return false;
}

void Request::appendToBody(std::string content)
{
	if (isBoundary(_boundary, content))
	{
		if (!_isArg) 
			_isArg = true;
		else
		{
			_args.push_back(parseArgument(_body));
			_body.clear();
		}
	}
	else if (isPreffix(_boundary, content))
	{
		_isArg = false;
		_args.push_back(parseArgument(_body));
		_body.clear();
		isDone = true;
	}
	else if (_isArg)
		_body.append(content).append("\n");
}

void Request::parseRequest()
{
	// parse transfer encoding / chunked data
	std::string buffer;
	std::istringstream lines(_data);

	isDone = false;
	while (std::getline(lines, buffer))
	{
		// log "current line: " << buffer line;
		if (!_method.length() && buffer.find("HTTP/1.1") != std::string::npos)
		{
			_method = buffer.substr(0, getSpaceIndex(buffer, 1) - 1);
			_uri = buffer.substr(getSpaceIndex(buffer, 1), getSpaceIndex(buffer, 2) - getSpaceIndex(buffer, 1) - 1);
			if (_uri.find("?") != std::string::npos)
				_query = _uri.substr(_uri.find("?") + 1);
			_protocol = buffer.substr(getSpaceIndex(buffer, 2));
			_protocol.pop_back();
		}
		else if (!_host.length() && buffer.find("Host") != std::string::npos)
		{
			_host = buffer.substr(buffer.find(":") + 2, buffer.length() - buffer.find(":") - 3);
		}
		else if (!_boundary.length() && buffer.find("Content-Type") != std::string::npos)
		{
			if (buffer.find("boundary=") != std::string::npos)
			{
				_ctype = buffer.substr(buffer.find_first_of(": ") + 2, buffer.find_first_of(";") - 14);
				_boundary = _boundary.append("--").append(buffer.substr(buffer.find("boundary=") + 9));
				_boundary.pop_back();
			}
			else
				_ctype = buffer.substr(buffer.find_first_of(": ") + 2, buffer.length() - buffer.find_first_of(":") - 3);
		}
		else if (!_clen && !_disp.length() && buffer.find("Content-Disposition") != std::string::npos)
			_disp = buffer.substr(buffer.find(":") + 2, buffer.length() - buffer.find(":") - 3);
		else if (!_clen && buffer.find("Content-Length") != std::string::npos)
			_clen = std::stoi(buffer.substr(buffer.find(":") + 2));
		else if (!_contype.length() && buffer.find("Connection") != std::string::npos)
			_contype = buffer.substr(buffer.find(":") + 2, buffer.length() - buffer.find(":") - 3);
		else if (buffer.find("Cookie:") != std::string::npos)
		{
			if (_cookies.length())
				_cookies.append(";");
			_cookies.append(buffer.substr(buffer.find(":") + 2));
			_cookies.pop_back();
		}
		else if (!_clen)
			parseHeader(buffer);
		else if (_clen && !_boundary.length())
		{
			if (_isArg)
			{
				_body.append(buffer);
				if (_body.length() == _clen)
				{
					Request::Argument arg = {};
					arg.data = _body;
					_args.push_back(arg);
					_body.clear();
					_isArg = false;
				}
				else
					_body.append("\n");
			}
			else
				_isArg = true;
		}
		else
			appendToBody(buffer);
	}
	if (checkDataDone())
		isDone = true;
	if (isDone)
	{
		int error = 0;
		if (_protocol.compare("HTTP/1.1") != 0)
			error = 1;
		if (_method.length() < 3)
			error = 1;
		if (!_uri.length() || _uri[0] != '/')
			error = 1;
		if (_method.compare("POST") == 0 && _clen && getPostBodyLength(_data) != _clen)
			error = 1;
		if (!_host.length())
			error = 1;
		_error = error;
		// if (_error)
			// log "ERROR in request" line;
	}
}

bool Request::checkDataDone()
{
	std::string buffer;
	std::istringstream lines(_data);
	bool _isDone = false;
	int len = 1;

	size_t i = _data.find("\r\n\r\n");
	if (i != std::string::npos)
	{
		if (_data.find("Content-Length:") != std::string::npos)
		{
			len = std::stoi(_data.substr(_data.find("Content-Length: ") + 16));
			std::string tmp = _data.substr(i + 4);
			if (tmp.length() == len)
				_isDone = true;
		}
		else
			_isDone = true;
	}
	return _isDone;
}

void Request::printRequest()
{
	log "method: " << _method << "|" line;
	log "uri: " << _uri << "|" line;
	log "query: " << _query << "|" line;
	log "host: " << _host << "|" line;
	log "protocol: " << _protocol << "|" line;
	log "content length: " << _clen << "|" line;
	log "content type: " << _ctype << "|" line;
	log "boundary: " << _boundary << "|" line;
	log "disposition: " << _disp << "|" line;
	log "connection type: " << _contype << "|" line;
	log "arguments size: " << _args.size() << "|" line;
	for (int i = 0; i < _args.size(); i++)
		log "argument disposition: " << _args[i].disp << "| " << "content type: " << _args[i].ctype << "| " << "data: " << _args[i].data << "|" line;
	log "" line;
}

void Request::clear()
{
	this->_data.clear();
	this->_method.clear();
	this->_uri.clear();
	this->_query.clear();
	this->_protocol.clear();
	this->_host.clear();
	this->_body.clear();
	this->_ctype.clear();
	this->_disp.clear();
	this->_boundary.clear();
	this->_args.clear();
	this->_contype.clear();
	this->_headers.clear();
	this->_cookies.clear();
	this->_clen = 0;
	this->_error = 0;
	this->isDone = false;
	this->_isArg = false;
}

const std::string &Request::getMethod() const
{
	return _method;
}

const std::string &Request::getUri() const
{
	return _uri;
}

unsigned int Request::getContentLen() const
{
	return _clen;
}

const std::string &Request::getContentType() const
{
	return _ctype;
}

const std::string &Request::getBody() const
{
	return _body;
}

const std::string &Request::getData() const
{
	return _data;
}

const std::string &Request::getConnectionType() const
{
	return _contype;
}

void Request::appendToData(std::string content)
{
	if (content[0] != '\r' || _data.length())
		_data.append(content);
}

size_t Request::getLenArguments()
{
	return _args.size();
}

std::vector<Request::Argument> Request::getArguments()
{
	return _args;
}

Request::Argument Request::getArgument(int i)
{
	return _args[i];
}

Connection *Request::getConnection()
{
	return _connection;
}

std::vector<Request::Header> Request::getHeaders()
{
	return _headers;
}

ServerData Request::getServerData()
{
	return this->_connection->getServer()->getData();
}

void Request::setUri(std::string const &uri)
{
	this->_uri = uri;
}

void Request::setConnectionType(std::string const &contype)
{
	this->_contype = contype;
}

int Request::getRequestError()
{
	return this->_error;
}

const std::string &Request::getProtocol() const
{
	return this->_protocol;
}

const std::string &Request::getHost() const
{
	return this->_host;
}

const std::string &Request::getCookies() const
{
	return this->_cookies;
}

const std::string &Request::getQuery() const
{
	return this->_query;
}