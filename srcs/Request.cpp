#include "Webserv.hpp"

Request::Request()
{
}

Request::Request(Connection *_connection) : _data(""), _clen(0), _boundary(""), _contype("keep-alive"), _connection(_connection), _isArg(false), _error(0), isDone(false)
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
	if (!s1.length() || !s2.length() || n1 > n2)
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

void Request::parseHeader(std::string &data)
{
	Request::Header header = {};

	if (data.length() > 1 && data.find(":") == std::string::npos)
		throw std::invalid_argument("invalid header");
	header.name = data.substr(0, data.find(":"));
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

bool isSet(std::string const &arg, int (*func)(int))
{
	for (size_t i = 0; i < arg.size(); i++)
	{
		if (!func(arg[i]))
			return false;
	}
	return true;
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

void Request::parseBody()
{
	std::string buffer;
	std::string tmp = _data.substr(_data.find("\r\n\r\n") + 4);
	std::istringstream lines(tmp);

	while (std::getline(lines, buffer)) {
		
		if (!_boundary.empty())
		{
			if (isBoundary(_boundary, buffer))
			{
				if (!_isArg)
					_isArg = true;
				else
				{
					_args.push_back(parseArgument(_body));
					_body.clear();
				}
			}
			else if (isPreffix(_boundary, buffer))
			{
				_isArg = false;
				_args.push_back(parseArgument(_body));
				_body.clear();
				isDone = true;
			}
			else if (_isArg)
				_body.append(buffer).append("\n");
		} else {
			if (_isArg)
			{
				buffer.pop_back();
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
	}
	_body = _data.substr(_data.find("\r\n\r\n") + 4);
}

void Request::parseRequest()
{
	std::string buffer;
	std::istringstream lines(_data);

	isDone = false;
	try {
		while (std::getline(lines, buffer))
		{
			if (!_method.length() && buffer.find("HTTP/1.1") != std::string::npos)
			{
				_method = buffer.substr(0, getSpaceIndex(buffer, 1) - 1);
				if (!isSet(_method, isupper))
					throw std::invalid_argument("invalid method");
				_uri = buffer.substr(getSpaceIndex(buffer, 1), getSpaceIndex(buffer, 2) - getSpaceIndex(buffer, 1) - 1);
				if (_uri.find("?") != std::string::npos)
				{
					_query = _uri.substr(_uri.find("?") + 1);
					_uri = _uri.substr(0, _uri.find("?"));
				}
				_protocol = buffer.substr(getSpaceIndex(buffer, 2));
				_protocol.pop_back();
			}
			else if (!_host.length() && buffer.find("Host") != std::string::npos)
			{
				if (buffer.find("Host: ") == std::string::npos)
					throw std::invalid_argument("invalid host header");
				_host = buffer.substr(buffer.find(":") + 2, buffer.length() - buffer.find(":") - 3);
				if (_host.find(":") != std::string::npos) {
					_host = _host.substr(0, _host.find(":"));
				}
			}
			else if (!_boundary.length() && buffer.find("Content-Type") != std::string::npos)
			{
				if (buffer.find("Content-Type: ") == std::string::npos)
					throw std::invalid_argument("invalid content_type header");
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
			{
				if (buffer.find("Content-Disposition: ") == std::string::npos)
					throw std::invalid_argument("invalid content-disposition header");
				_disp = buffer.substr(buffer.find(":") + 2, buffer.length() - buffer.find(":") - 3);
			}
			else if (!_clen && buffer.find("Content-Length") != std::string::npos)
			{
				buffer.pop_back();
				std::string _lenstr =  buffer.substr(buffer.find(":") + 2);
				if (buffer.find("Content-Length") != 0 || buffer.find("Content-Length: ") == std::string::npos ||
					 _lenstr.length() == 0 || !isSet(_lenstr, std::isdigit))
					throw std::invalid_argument("invalid content-length header");
				_clen = std::stoi(buffer.substr(buffer.find(":") + 2));
			}
			else if (buffer.find("Connection") != std::string::npos)
			{
				if (buffer.find("Connection: ") == std::string::npos)
					throw std::invalid_argument("invalid connection header");
				_contype = buffer.substr(buffer.find(":") + 2, buffer.length() - buffer.find(":") - 3);
			}
			else if (buffer[0] == '\r')
				break ;
			else
				parseHeader(buffer);
		}
		if (_data.find("\r\n\r\n") != std::string::npos)
			parseBody();
		if (checkDataDone())
			isDone = true;
	} catch (std::exception &e)
	{
		outputLogs("exception at request parsing: " + std::string(e.what()));
		isDone = true;
		_error = 1;
	}
	if (isDone && !_error)
	{
		int error = 0;
		if (_protocol.compare("HTTP/1.1") != 0)
			error = 1;
		if (_method.length() < 3)
			error = 1;
		if (!_uri.length() || _uri[0] != '/')
			error = 1;
		if (_method.compare("POST") == 0)
		{
			if (_data.find("Content-Length:") == std::string::npos)
				error = 1;
			if (_clen && !validateContentLength())
				error = 1;
		} else if (_method.compare("DELETE") == 0) {
			if (_data.find("Content-Length:") != std::string::npos && _clen && validateContentLength())
				error = 1;
		}
		if (!_host.length())
			error = 1;
		_error = error;
	}
}

bool is_hex_notation(std::string &s)
{
	return s.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos;
}

std::string parseChunked(std::string &content)
{
	std::string data;
	std::string buffer;
	int len;

	std::istringstream lines(content);
	while (std::getline(lines, buffer))
	{
		buffer.pop_back();
		if (len) {
			data.append(buffer.substr(0, len));
			len = 0;
		} else {
			if (!is_hex_notation(buffer))
				throw std::invalid_argument("not received a hex value");
			try {
				len = std::stoi(buffer);
			} catch (std::exception &e) {}
		}
	}
	return data;
}

bool Request::validateContentLength()
{
	size_t len;
	bool _isDone = false;
	size_t i = _data.find("\r\n\r\n");

	len = std::stoi(_data.substr(_data.find("Content-Length: ") + 16));
	std::string tmp = _data.substr(i + 4);
	if (tmp[0] == '\r')
		throw std::invalid_argument("bad request");
	if (!tmp.length())
		return false;
	if (isSuffix("\r\n\r\n", tmp) && _data.find("boundary") == std::string::npos) {
		tmp.erase(tmp.end() - 4, tmp.end());
		len += std::count(tmp.begin(), tmp.end(), '\r');
		_isDone = true;
		if (len != tmp.length())
			_error = 1;
	} else if (tmp.length() == len && (tmp.find("\r") == std::string::npos || _data.find("boundary") != std::string::npos))
		_isDone = true;
	return _isDone;
}

bool Request::checkDataDone()
{
	std::string buffer;
	std::istringstream lines(_data);
	bool _isDone = false;
	std::string chunked;

	size_t i = _data.find("\r\n\r\n");
	if (_data.find(4) != std::string::npos)
	{
		setConnectionType("close");
		_isDone = true;
	}
	if (i != std::string::npos)
	{
		if (_data.find("Transfer-Encoding: chunked") != std::string::npos)
		{
			std::string tmp = _data.substr(i + 4);
			if (tmp.find("0\r\n\r\n") != std::string::npos) {
				try {
					chunked = parseChunked(tmp);
					_data = _data.substr(0, i);
					_data.erase(_data.find("Transfer-Encoding"), 26);
					_data.append("Content-Length: ").append(std::to_string(chunked.length())).append("\r\n\r\n");
					_data.append(chunked).append("\r\n\r\n");
				} catch (std::exception &e) {
					outputLogs("exception at checkDataDone: " + std::string(e.what()));
					_error = 1;
				}
				_isDone = true;
			} else if (tmp[0] == '\r') {
				_error = 1;
				_isDone = true;
			}
		}
		else if (_data.find("Content-Length:") != std::string::npos)
		{
			try {
				_isDone = validateContentLength();
			} catch (std::exception &e)
			{
				_isDone = true;
				_error = 1;
			}
		}
		else
			_isDone = true;
	}
	return _isDone;
}

void Request::printRequest()
{
	outputLogs("\n\n[++++]  NEW REQUEST  [++++] \n"+ _data);
	outputLogs("method: " + _method);
	outputLogs("uri: " + _uri);
	outputLogs("host: " + _host);
	outputLogs("protocol: " + _protocol);
	outputLogs("content-length: " + std::to_string(_clen));
	outputLogs("content-type: " + _ctype);
	outputLogs("disposition: " + _disp);
	outputLogs("connection type: " + _contype);
	outputLogs("headers size: " + std::to_string(_headers.size()));
	for (size_t i = 0; i < _headers.size(); i++)
		outputLogs(_headers[i].name + "=" + _headers[i].value);
	outputLogs("argument size: " + std::to_string(_args.size()));
	for (size_t i = 0; i < _args.size(); i++)
		outputLogs("disp: " + _args[i].disp + "| type: " + _args[i].ctype + "| data: " + _args[i].data);
	outputLogs("[----]  END REQUEST  [----]");
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

int Request::getContentLen() const
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

void Request::appendToData(char *content)
{
	if (content[0] != '\r' || _data.length())
	{
		_data.append(content);
	}
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

std::vector<ServerData> Request::getServerData(std::string &name)
{
	return this->_connection->getServer()->getData(name);
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

const std::string &Request::getQuery() const
{
	return this->_query;
}
