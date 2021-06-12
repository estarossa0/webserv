#include "websrv.h"

std::string const ConfigParser::primitives_openings[NUMBER_OF_SERVER_PRIMITIVES] = {
	PORT_OP,
	HOST_OP,
	SERVER_NAME_OP,
	CLIENT_MAX_SIZE_BODY_OP,
	ERROR_PAGE_OP,
	ROOT_OP,
	LOCATION_OP,
};

std::string const ConfigParser::location_identifiers[NUMBER_OF_LOCATION_PRIMITIVES] = {
	LOC_ROOT,
	LOC_AUTOINDEX,
	LOC_INDEX,
	LOC_ALLOWED_METHODS,
	LOC_RETURN,
	LOC_CGI,
	UPLOAD_LOC_ENABLE,
	UPLOAD_LOC_STORE,
};

ConfigParser::ConfigParser(char const *inFilename) : _filename(inFilename)
{
	std::fstream output("./outputParser", std::ios::out | std::ios::trunc);
	_getFileContent();
	output << "got file content successfully" << std::endl;
	if (output.is_open())
	{
		for (size_t i = 0; i < _fileLines.size(); i++)
		{
			output << _fileLines[i] << std::endl;
		}
	}
	_indexServers();

	output << std::endl
		   << "\nservers has been indexed successfully" << std::endl
		   << std::endl;
	output << "======================== SERVERS INDEXING ==========================";
	output << std::endl
		   << std::endl;

	for (size_t i = 0; i < _serversIndexing.size(); i += 2)
	{
		output << "start = [" << _serversIndexing[i] << "], end[" << _serversIndexing[i + 1] << "]" << std::endl;
	}

	_parseContent();
	output << "content has been parsed successfully" << std::endl;
	// std::cout << "content has been parsed successfully" << std::endl;

	output << "======================== SERVERS PARSING ==========================" << std::endl;

	for (size_t i = 0; i < _servers.size(); i++)
	{
		output << _servers[i] << std::endl;
	}
}

ConfigParser::~ConfigParser()
{
	_servers.clear();
	_fileLines.clear();
	_serversIndexing.clear();
	_checked_primitives.clear();
	_checked_location_primitives.clear();
}

std::vector<ServerData> const &ConfigParser::getServers() const
{
	return this->_servers;
}

void ConfigParser::_trim(std::string &str)
{
	while (!str.empty() && isspace(str.front()))
		str.erase(0, 1);
	while (!str.empty() && isspace(str.back()))
		str.erase(str.size() - 1, 1);
}

std::vector<std::string> ConfigParser::_split(std::string const &_line)
{
	std::vector<std::string> wordsArr;
	std::stringstream ss(_line);
	std::string buff;

	while (ss >> buff)
		wordsArr.push_back(buff);
	return wordsArr;
}

std::vector<std::string> ConfigParser::_split(std::string const &_line, char c)
{
	std::vector<std::string> wordsArr;
	std::stringstream ss(_line);
	std::string buff;

	while (getline(ss, buff, c))
		wordsArr.push_back(buff);
	return wordsArr;
}

void ConfigParser::_getFileContent()
{
	std::vector<std::string> fileParts = _split(_filename, '.');
	if (fileParts.back() != "conf")
		throw std::invalid_argument(ERROR_FILE_EXTENSION);

	std::ifstream inFile(_filename);
	std::string buff;

	if (!inFile.is_open())
		throw std::invalid_argument(ERROR_FILE);
	size_t hashPos;
	while (std::getline(inFile, buff))
	{
		_trim(buff);
		if (buff[0] == '#')
			continue;
		if ((hashPos = buff.find('#')) != std::string::npos)
			buff = buff.substr(0, hashPos);
		if (!buff.empty())
			_fileLines.push_back(buff);
	}

	inFile.close();
}

void ConfigParser::_indexServers()
{
	int isBracesValid = 0;
	bool serverBraceOpen = false;
	size_t pos;
	size_t count;

	for (size_t i = 0; i < _fileLines.size(); i++)
	{
		// doubling braces in the same _line error
		count = std::count(_fileLines[i].begin(), _fileLines[i].end(), '{') +
				std::count(_fileLines[i].begin(), _fileLines[i].end(), '}');
		if (count > 1)
			throw std::runtime_error(ERROR_DOUBLE_BRACE);
		else if (count == 1 && _fileLines[i].size() > 1)
			throw std::runtime_error(ERROR_BRACE_NOT_ALONE + _fileLines[i]);

		if ((pos = _fileLines[i].find(OPENNING_BRACE)) != std::string::npos)
		{
			isBracesValid++;
		}
		else if ((pos = _fileLines[i].find(CLOSING_BRACE)) != std::string::npos)
		{
			if (--isBracesValid < 0)
				throw std::runtime_error(ERROR_BRACES);
		}

		if (serverBraceOpen && isBracesValid == 0)
		{
			serverBraceOpen = false;
			_serversIndexing.push_back(i);
		}

		if (_fileLines[i].compare(SERVER_OP) == 0)
		{
			if (serverBraceOpen == true)
				throw std::runtime_error(ERROR_DEFINE_SERVER_INSIDE_SERVER);
			serverBraceOpen = true;
			if (i + 1 < _fileLines.size() && _fileLines[i + 1].compare(OPENNING_BRACE) == 0)
			{
				_serversIndexing.push_back(i + 1);
			}
			else
				throw std::runtime_error(ERROR_BRACES);
		}
	}
	if (serverBraceOpen || isBracesValid)
		throw std::runtime_error(ERROR_BRACES);
	if (_serversIndexing.size() == 0)
		throw std::runtime_error(ERROR_EMPTY_CONFIGURATION);
}

void ConfigParser::_parseContent()
{
	size_t start;
	size_t end;
	int parserIndex;
	int doneParsingIndex;
	ParserFuncPtr _server_primitive_parser[NUMBER_OF_SERVER_PRIMITIVES] = {
		&ConfigParser::_portParser,
		&ConfigParser::_hostParser,
		&ConfigParser::_serverNameParser,
		&ConfigParser::_clientBodySizeParser,
		&ConfigParser::_errorPageParser,
		&ConfigParser::_rootDirParser,
		&ConfigParser::_locationParser,
	};

	for (size_t i = 0; i < NUMBER_OF_SERVER_PRIMITIVES; i++)
		_checked_primitives.insert(std::pair<std::string, bool>(primitives_openings[i], false));
	for (size_t i = 0; i < NUMBER_OF_LOCATION_PRIMITIVES; i++)
		_checked_primitives.insert(std::pair<std::string, bool>(location_identifiers[i], false));

	size_t i = 0;
	while (i < _serversIndexing.size())
	{
		start = _serversIndexing[i] + 1;
		end = _serversIndexing[i + 1];
		if (start >= end)
			throw std::runtime_error(ERROR_EMPTY_SERVER_CONFIGURATION);

		ServerData sv = ServerData();
		for (size_t i = 0; i < NUMBER_OF_SERVER_PRIMITIVES; i++)
			_checked_primitives[primitives_openings[i]] = false;

		while (start < end)
		{
			if ((parserIndex = _isPrimitive(_fileLines[start])) >= 0)
			{
				if (primitives_openings[parserIndex] != ERROR_PAGE_OP &&
					primitives_openings[parserIndex] != LOCATION_OP &&
					_checked_primitives[primitives_openings[parserIndex]])
					throw std::runtime_error(ERROR_SERVER_DUPLICATED_FIELD + _fileLines[start]);

				_checked_primitives[primitives_openings[parserIndex]] = true;

				if ((doneParsingIndex = (this->*_server_primitive_parser[parserIndex])(start, sv)) == end + 1)
					throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[start]);
				start = static_cast<size_t>(doneParsingIndex);
				continue;
			}
			else if (_fileLines[start].compare(OPENNING_BRACE) && _fileLines[start].compare(CLOSING_BRACE))
				throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[start]);
			start++;
		}
		if (!sv.hasNecessaryElements())
			throw std::runtime_error(ERROR_MISSING_NECESSARY_ELEMENT);
		if (sv.getLocations().size() == 0)
			sv.addLocation(Location());
		_servers.push_back(sv);
		i += 2;
	}
}

int ConfigParser::_isPrimitive(std::string const &_line)
{
	for (size_t i = 0; i < NUMBER_OF_SERVER_PRIMITIVES; i++)
	{
		if (_line.compare(0, primitives_openings[i].size(), primitives_openings[i]) == 0)
		{
			return i;
		}
	}
	return -1;
}

int ConfigParser::_isLocationPrimitive(std::string const &_line)
{
	for (size_t i = 0; i < NUMBER_OF_LOCATION_PRIMITIVES; i++)
	{
		if (_line.compare(0, location_identifiers[i].size(), location_identifiers[i]) == 0)
		{
			return i;
		}
	}
	return -1;
}

bool ConfigParser::_isSet(std::string const &arg, int (*func)(int))
{
	for (size_t i = 0; i < arg.size(); i++)
	{
		if (!func(arg[i]))
			return false;
	}
	return true;
}

void ConfigParser::_semicolonChecker(std::string &_line)
{
	_trim(_line);
	if (_line.back() != ';')
		throw std::runtime_error(ERROR_MISSING_SEMICOLON + _line);
	if (std::count(_line.begin(), _line.end(), ';') > 1)
		throw std::runtime_error(ERROR_DOUBLE_SEMICOLON);
	_line.erase(_line.end() - 1);
	_trim(_line);
}

std::string const getStringType(char const *_line)
{
	return std::string(_line);
}

int ConfigParser::_portParser(size_t index, ServerData &sv)
{
	std::string _line = _fileLines[index];

	_semicolonChecker(_line);

	std::vector<std::string> tokens = _split(_line);
	if (tokens.size() == 2)
	{
		if (tokens[0] != PORT_OP)
			throw std::runtime_error(DID_YOU_MEAN + getStringType(PORT_OP) + IN_THIS_LINE + _fileLines[index] + " ?");
		if (!_isSet(tokens[1], &(std::isdigit)))
			throw std::runtime_error(ERROR_PORT_NAN);
		sv.setPort(std::atoi(tokens[1].c_str()));
	}
	else
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
	return index + 1;
}

int ConfigParser::_hostParser(size_t index, ServerData &sv)
{
	std::string _line = _fileLines[index];

	_semicolonChecker(_line);

	std::vector<std::string> tokens = _split(_line);
	if (tokens.size() == 2)
	{
		if (tokens[0] != HOST_OP)
			throw std::runtime_error(DID_YOU_MEAN + getStringType(HOST_OP) + IN_THIS_LINE + _fileLines[index] + " ?");
		sv.setHost(tokens[1]);
	}
	else
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
	return index + 1;
}

int ConfigParser::_serverNameParser(size_t index, ServerData &sv)
{
	std::string _line = _fileLines[index];

	_semicolonChecker(_line);

	std::vector<std::string> tokens = _split(_line);
	if (tokens.size() == 2)
	{
		if (tokens[0] != SERVER_NAME_OP)
			throw std::runtime_error(DID_YOU_MEAN + getStringType(SERVER_NAME_OP) + IN_THIS_LINE + _fileLines[index] + " ?");
		sv.setName(tokens[1]);
	}
	else
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
	return index + 1;
}

int ConfigParser::_clientBodySizeParser(size_t index, ServerData &sv)
{
	std::string _line = _fileLines[index];

	_semicolonChecker(_line);

	//check unity name 'm' [megabytes]
	if (_line.back() != 'm')
		throw std::runtime_error(ERROR_CLIENT_BODY_SIZE_UNITY);
	_line.erase(_line.end() - 1);

	std::vector<std::string> tokens = _split(_line);
	if (tokens.size() == 2)
	{
		if (tokens[0] != CLIENT_MAX_SIZE_BODY_OP)
			throw std::runtime_error(DID_YOU_MEAN + getStringType(CLIENT_MAX_SIZE_BODY_OP) + IN_THIS_LINE + _fileLines[index] + " ?");
		if (!_isSet(tokens[1], &(std::isdigit)))
			throw std::runtime_error(ERROR_CLIENT_BODY_SIZE_NAN);
		sv.setClientBodySize(std::atoi(tokens[1].c_str()));
	}
	else
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
	return index + 1;
}

int ConfigParser::_errorPageParser(size_t index, ServerData &sv)
{
	std::string _line = _fileLines[index];

	_semicolonChecker(_line);

	std::vector<std::string> tokens = _split(_line);
	if (tokens.size() == 3)
	{
		if (tokens[0] != ERROR_PAGE_OP)
			throw std::runtime_error(DID_YOU_MEAN + getStringType(ERROR_PAGE_OP) + IN_THIS_LINE + _fileLines[index] + " ?");
		if (!_isSet(tokens[1], &(std::isdigit)))
			throw std::runtime_error(ERROR_ERRPAGE_CODE_NAN);
		sv.addErrorPage(std::atoi(tokens[1].c_str()), tokens[2]);
	}
	else
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
	return index + 1;
}

int ConfigParser::_rootDirParser(size_t index, ServerData &sv)
{
	std::string _line = _fileLines[index];

	_semicolonChecker(_line);

	std::vector<std::string> tokens = _split(_line);
	if (tokens.size() == 2)
	{
		if (tokens[0] != ROOT_OP)
			throw std::runtime_error(DID_YOU_MEAN + getStringType(ROOT_OP) + IN_THIS_LINE + _fileLines[index] + " ?");
		sv.setRootDir(tokens[1]);
	}
	else
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
	return index + 1;
}

int ConfigParser::_locationParser(size_t index, ServerData &sv)
{
	if (_fileLines[index].back() == ';')
		throw std::runtime_error(ERROR_LOCATION_WITH_SEMICOLON + _fileLines[index]);
	Location loc = Location();

	_locationPathParser(index, loc);

	int parserIndex;
	bool isEmpty = _fileLines[++index].compare(CLOSING_BRACE) == 0;

	LocationFieldParserFuncPtr _location_primitive_parser[NUMBER_OF_LOCATION_PRIMITIVES] = {
		&ConfigParser::_locRootDirParser,
		&ConfigParser::_locAutoIndexParser,
		&ConfigParser::_locIndexParser,
		&ConfigParser::_locAllowedMethodsParser,
		&ConfigParser::_locRedirectionParser,
		&ConfigParser::_locCGIParser,
		&ConfigParser::_locUploadEnableParser,
		&ConfigParser::_locUploadLocationParser,
	};

	for (size_t i = 0; i < NUMBER_OF_LOCATION_PRIMITIVES; i++)
		_checked_location_primitives[location_identifiers[i]] = false;
	while (_fileLines[index].compare(CLOSING_BRACE))
	{
		if ((parserIndex = _isLocationPrimitive(_fileLines[index])) >= 0)
		{
			if (_checked_location_primitives[location_identifiers[parserIndex]])
				throw std::runtime_error(ERROR_LOCATION_DUPLICATED_FIELD + _fileLines[index]);
			(this->*_location_primitive_parser[parserIndex])(index, loc);
			_checked_location_primitives[location_identifiers[parserIndex]] = true;
		}
		else
			throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
		index++;
	}

	if (isEmpty)
		throw std::runtime_error(ERROR_EMPTY_LOCATION_CONFIG);
	if (loc.isCGI() && loc.getFastCgiPass().empty())
		throw std::runtime_error(ERROR_CGI_NOT_FOUND);
	sv.addLocation(loc);

	return index + 1;
}

void ConfigParser::_locationPathParser(size_t &index, Location &loc)
{
	std::string _line = _fileLines[index];

	std::vector<std::string> tokens = _split(_line);
	std::vector<std::string> insideTokens;
	if (tokens.size() == 2)
	{
		if (tokens[0] != LOCATION_OP)
			throw std::runtime_error(DID_YOU_MEAN + getStringType(LOCATION_OP) + IN_THIS_LINE + _fileLines[index] + " ?");
		if (tokens[1].front() == '*')
		{
			insideTokens = _split(tokens[1], '.');
			if (insideTokens.size() != 2 || insideTokens[0] != "*" || !_isSet(insideTokens[1], std::isalpha))
				throw std::runtime_error(ERROR_CGI_EXTENSION_ERROR + _fileLines[index]);
			loc.setPath(insideTokens[1]);
			loc.setIsCGI(true);
		}
		else
			loc.setPath(tokens[1]);
	}
	else
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
	if (_fileLines[++index] != OPENNING_BRACE)
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
}

void ConfigParser::_locRootDirParser(size_t index, Location &loc)
{
	std::string _line = _fileLines[index];

	_semicolonChecker(_line);

	std::vector<std::string> tokens = _split(_line);
	if (tokens.size() == 2)
	{
		if (tokens[0] != LOC_ROOT)
			throw std::runtime_error(DID_YOU_MEAN + getStringType(LOC_ROOT) + IN_THIS_LINE + _fileLines[index] + " ?");
		loc.setRootDir(tokens[1]);
	}
	else
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
}

void ConfigParser::_locAutoIndexParser(size_t index, Location &loc)
{
	std::string _line = _fileLines[index];

	_semicolonChecker(_line);

	std::vector<std::string> tokens = _split(_line);
	if (tokens.size() == 2)
	{
		if (tokens[0] != LOC_AUTOINDEX)
			throw std::runtime_error(DID_YOU_MEAN + getStringType(LOC_AUTOINDEX) + IN_THIS_LINE + _fileLines[index] + " ?");
		if (tokens[1].compare("on") && tokens[1].compare("off"))
			throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
		loc.setAutoIndex((tokens[1] == "on"));
	}
	else
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
}

void ConfigParser::_locIndexParser(size_t index, Location &loc)
{
	std::string _line = _fileLines[index];

	_semicolonChecker(_line);

	std::vector<std::string> tokens = _split(_line);
	if (tokens.size() < 2)
	{
		if (tokens[0] != LOC_INDEX)
			throw std::runtime_error(DID_YOU_MEAN + getStringType(LOC_INDEX) + IN_THIS_LINE + _fileLines[index] + " ?");
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
	}
	tokens.erase(tokens.begin());
	loc.setDefaultFiles(tokens);
}

void ConfigParser::_locAllowedMethodsParser(size_t index, Location &loc)
{
	std::stringstream ss(_fileLines[index]);
	std::string buff;
	ss >> buff;
	if (buff != LOC_ALLOWED_METHODS)
		throw std::runtime_error(DID_YOU_MEAN + getStringType(LOC_ALLOWED_METHODS) + IN_THIS_LINE + _fileLines[index] + " ?");

	std::string _line = _fileLines[index].substr(strlen(LOC_ALLOWED_METHODS));

	_semicolonChecker(_line);

	if (_line.empty() || std::count(_line.begin(), _line.end(), OPENNING_BRACKET) != 1 ||
		std::count(_line.begin(), _line.end(), CLOSING_BRACKET) != 1 ||
		_line.back() != CLOSING_BRACKET || _line.front() != OPENNING_BRACKET)
		throw std::runtime_error(ERROR_ALLOWED_METHODS_SYNTAX + _fileLines[index]);

	// remove the brackets at front and back
	_line.erase(0, 1);
	_line.erase(_line.size() - 1, 1);
	// get the allowed methods by splitting with comma
	std::vector<std::string> tokens = _split(_line, ',');

	//  1 <= number of methods <= 3 and number of commas + 1 = number of methods
	if (tokens.size() < 1 || tokens.size() > 3 ||
		tokens.size() != std::count(_line.begin(), _line.end(), ',') + 1)
		throw std::runtime_error(ERROR_ALLOWED_METHODS_SYNTAX + _fileLines[index]);
	for (size_t i = 0; i < tokens.size(); i++)
	{
		_trim(tokens[i]);
		if (tokens[i].empty())
			throw std::runtime_error(ERROR_ALLOWED_METHODS_SYNTAX + _fileLines[index]);
	}
	loc.setAllowedMethods(tokens);
}

void ConfigParser::_locUploadEnableParser(size_t index, Location &loc)
{
	std::string _line = _fileLines[index];

	_semicolonChecker(_line);

	std::vector<std::string> tokens = _split(_line);
	if (tokens.size() == 2)
	{
		if (tokens[0] != UPLOAD_LOC_ENABLE)
			throw std::runtime_error(DID_YOU_MEAN + getStringType(UPLOAD_LOC_ENABLE) + IN_THIS_LINE + _fileLines[index] + " ?");
		if (tokens[1] != "on" && tokens[1] != "off")
			throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
		loc.setUploadEnabled((tokens[1] == "on"));
	}
	else
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
}

void ConfigParser::_locUploadLocationParser(size_t index, Location &loc)
{
	std::string _line = _fileLines[index];

	_semicolonChecker(_line);

	std::vector<std::string> tokens = _split(_line);
	if (tokens.size() == 2)
	{
		if (tokens[0] != UPLOAD_LOC_STORE)
			throw std::runtime_error(DID_YOU_MEAN + getStringType(UPLOAD_LOC_STORE) + IN_THIS_LINE + _fileLines[index] + " ?");
		loc.setUploadLocation(tokens[1]);
	}
	else
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
}

void ConfigParser::_locRedirectionParser(size_t index, Location &loc)
{
	std::string _line = _fileLines[index];

	_semicolonChecker(_line);

	std::vector<std::string> tokens = _split(_line);
	if (tokens.size() == 3)
	{
		if (tokens[0] != LOC_RETURN)
			throw std::runtime_error(DID_YOU_MEAN + getStringType(LOC_RETURN) + IN_THIS_LINE + _fileLines[index] + " ?");
		if (!_isSet(tokens[1], &(std::isdigit)))
			throw std::runtime_error(ERROR_RETURN_CODE_NAN);
		loc.setReturnCode(std::atoi(tokens[1].c_str()));
		loc.setReturnUrl(tokens[2]);
	}
	else
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
}

void ConfigParser::_locCGIParser(size_t index, Location &loc)
{
	std::string _line = _fileLines[index];

	_semicolonChecker(_line);

	std::vector<std::string> tokens = _split(_line);
	if (tokens.size() == 2)
	{
		if (tokens[0] != LOC_CGI)
			throw std::runtime_error(DID_YOU_MEAN + getStringType(LOC_CGI) + IN_THIS_LINE + _fileLines[index] + " ?");
		loc.setFastCgiPass(tokens[1]);
	}
	else
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
}
