#include "ConfigParser.hpp"

std::string const parser::ConfigParser::primitives_openings[NUMBER_OF_SERVER_PRIMITIVES] = {
	PORT_OP,
	HOST_OP,
	SERVER_NAME_OP,
	CLIENT_MAX_SIZE_BODY_OP,
	ERROR_PAGE_OP,
	ROOT_OP,
	LOCATION_OP,
};

std::string const parser::ConfigParser::location_identifiers[NUMBER_OF_LOCATION_PRIMITIVES] = {
	LOC_ROOT,
	LOC_AUTOINDEX,
	LOC_INDEX,
	LOC_ALLOWED_METHODS,
	LOC_RETURN,
	LOC_CGI,
	UPLOAD_LOC_ENABLE,
	UPLOAD_LOC_STORE,
};

parser::ConfigParser::ConfigParser(char const *inFilename) : _filename(inFilename)
{
	_getFileContent();
	std::cout << "got file content successfully" << std::endl;
	std::fstream output("./outputParser", std::ios::out | std::ios::trunc);
	if (output.is_open())
	{
		for (size_t i = 0; i < _fileLines.size(); i++)
		{
			output << _fileLines[i] << std::endl;
		}
	}
	_indexServers();
	std::cout << "servers has been indexed successfully" << std::endl;

	for (size_t i = 0; i < _serversIndexing.size(); i += 2)
	{
		output << "start = [" << _serversIndexing[i] << "], end[" << _serversIndexing[i + 1] << "]" << std::endl;
	}

	_parseContent();
	std::cout << "content has been parsed successfully" << std::endl;
}

parser::ConfigParser::~ConfigParser()
{
	_servers.clear();
	_fileLines.clear();
	_serversIndexing.clear();
}

std::vector<parser::Server> const &parser::ConfigParser::getServers() const
{
	return this->_servers;
}

void parser::ConfigParser::_trim(std::string &str)
{
	size_t start = 0;
	size_t end = str.size() > 0 ? str.size() - 1 : 0;

	while (isspace(str[start]))
		start++;
	while (isspace(str[end]))
		end--;

	str = str.substr(start, end - start + 1);
}

std::vector<std::string> parser::ConfigParser::_split(std::string const &line)
{
	std::vector<std::string> wordsArr;
	std::stringstream ss(line);
	std::string buff;

	while (ss >> buff)
		wordsArr.push_back(buff);
	return wordsArr;
}

std::vector<std::string> parser::ConfigParser::_split(std::string const &line, char c)
{
	std::vector<std::string> wordsArr;
	std::stringstream ss(line);
	std::string buff;

	while (getline(ss, buff, c))
		wordsArr.push_back(buff);
	return wordsArr;
}

void parser::ConfigParser::_getFileContent()
{
	std::ifstream inFile(_filename);
	std::string buff;

	if (!inFile.is_open())
	{
		throw std::invalid_argument(ERROR_FILE);
	}

	while (std::getline(inFile, buff))
	{
		_trim(buff);
		if (buff[0] == '#')
			continue;
		if (!buff.empty())
			_fileLines.push_back(buff);
	}

	inFile.close();
}

void parser::ConfigParser::_indexServers()
{
	int isBracesValid = 0;
	bool serverBraceOpen = false;
	size_t pos;
	size_t count;

	for (size_t i = 0; i < _fileLines.size(); i++)
	{
		// doubling braces in the same line error
		count = std::count(_fileLines[i].begin(), _fileLines[i].end(), '{') +
				std::count(_fileLines[i].begin(), _fileLines[i].end(), '}');
		if (count > 1)
			throw std::runtime_error(ERROR_DOUBLE_BRACE);
		else if (count == 1 && _fileLines[i].size() > 1)
			throw std::runtime_error(ERROR_DOUBLE_BRACE);

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

void parser::ConfigParser::_parseContent()
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
	{
		_checked_primitives.insert(std::pair<std::string, bool>(primitives_openings[i], false));
	}
	size_t i = 0;
	while (i < _serversIndexing.size())
	{
		start = _serversIndexing[i] + 1;
		end = _serversIndexing[i + 1];
		if (start >= end)
			throw std::runtime_error(ERROR_EMPTY_SERVER_CONFIGURATION);
		Server sv;
		while (start < end)
		{
			if ((parserIndex = _isPrimitive(_fileLines[start])) >= 0)
			{
				if ((doneParsingIndex = (this->*_server_primitive_parser[parserIndex])(start, sv)) >= end)
					throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[start]);
				start = static_cast<size_t>(doneParsingIndex);
				continue;
			}
			else if (_fileLines[start].compare(OPENNING_BRACE) && _fileLines[start].compare(CLOSING_BRACE))
			{

				throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[start]);
			}
			start++;
		}
		i += 2;
	}
}

int parser::ConfigParser::_isPrimitive(std::string const &line)
{
	for (size_t i = 0; i < NUMBER_OF_SERVER_PRIMITIVES; i++)
	{
		if (line.compare(0, primitives_openings[i].size(), primitives_openings[i]) == 0)
		{
			return i;
		}
	}
	return -1;
}

int parser::ConfigParser::_isLocationPrimitive(std::string const &line)
{
	for (size_t i = 0; i < NUMBER_OF_LOCATION_PRIMITIVES; i++)
	{
		if (line.compare(0, location_identifiers[i].size(), location_identifiers[i]) == 0)
		{
			return i;
		}
	}
	return -1;
}

bool parser::ConfigParser::_isSet(std::string const &arg, int (*func)(int))
{
	for (size_t i = 0; i < arg.size(); i++)
	{
		if (!func(arg[i]))
			return false;
	}
	return true;
}

void parser::ConfigParser::_semicolonChecker(std::string &line)
{
	if (line.back() != ';')
		throw std::runtime_error(ERROR_MISSING_SEMICOLON + line);
	if (std::count(line.begin(), line.end(), ';') > 1)
		throw std::runtime_error(ERROR_DOUBLE_SEMICOLON);
	line.erase(line.end() - 1);
}

int parser::ConfigParser::_portParser(size_t index, Server &sv)
{
	std::string line = _fileLines[index];

	_semicolonChecker(line);

	std::vector<std::string> tokens = _split(line);
	if (tokens.size() == 2)
	{
		if (!_isSet(tokens[1], &(std::isdigit)))
			throw std::runtime_error(ERROR_PORT_NAN);
		sv.setPort(std::atoi(tokens[1].c_str()));
		_checked_primitives[PORT_OP] = true;
	}
	else
	{
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
	}
	return index + 1;
}

int parser::ConfigParser::_hostParser(size_t index, Server &sv)
{
	std::string line = _fileLines[index];

	_semicolonChecker(line);

	std::vector<std::string> tokens = _split(line);
	if (tokens.size() == 2)
	{
		sv.setHost(tokens[1]);
		_checked_primitives[HOST_OP] = true;
	}
	else
	{
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
	}
	return index + 1;
}

int parser::ConfigParser::_serverNameParser(size_t index, Server &sv)
{
	std::string line = _fileLines[index];

	_semicolonChecker(line);

	std::vector<std::string> tokens = _split(line);
	if (tokens.size() == 2)
	{
		sv.setName(tokens[1]);
		_checked_primitives[SERVER_NAME_OP] = true;
	}
	else
	{
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
	}
	return index + 1;
}

int parser::ConfigParser::_clientBodySizeParser(size_t index, Server &sv)
{
	std::string line = _fileLines[index];

	_semicolonChecker(line);

	//check unity name 'm' [megabytes]
	if (line.back() != 'm')
		throw std::runtime_error(ERROR_CLIENT_BODY_SIZE_UNITY);
	line.erase(line.end() - 1);

	std::vector<std::string> tokens = _split(line);
	if (tokens.size() == 2)
	{
		if (!_isSet(tokens[1], &(std::isdigit)))
			throw std::runtime_error(ERROR_CLIENT_BODY_SIZE_NAN);
		sv.setClientBodySize(std::atoi(tokens[1].c_str()));
		_checked_primitives[CLIENT_MAX_SIZE_BODY_OP] = true;
	}
	else
	{
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
	}
	return index + 1;
}

int parser::ConfigParser::_errorPageParser(size_t index, Server &sv)
{
	std::string line = _fileLines[index];

	_semicolonChecker(line);

	std::vector<std::string> tokens = _split(line);
	if (tokens.size() == 3)
	{
		if (!_isSet(tokens[1], &(std::isdigit)))
			throw std::runtime_error(ERROR_ERRPAGE_CODE_NAN);
		sv.addErrorPage(std::atoi(tokens[1].c_str()), tokens[2]);
		_checked_primitives[ERROR_PAGE_OP] = true;
	}
	else
	{
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
	}
	return index + 1;
}

int parser::ConfigParser::_rootDirParser(size_t index, Server &sv)
{
	std::string line = _fileLines[index];

	_semicolonChecker(line);

	std::vector<std::string> tokens = _split(line);
	if (tokens.size() == 2)
	{
		sv.setRootDir(tokens[1]);
		_checked_primitives[ROOT_OP] = true;
	}
	else
	{
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
	}
	return index + 1;
}

int parser::ConfigParser::_locationParser(size_t index, Server &sv)
{

	if (_fileLines[index].back() == ';')
		throw std::runtime_error(ERROR_LOCATION_WITH_SEMICOLON + _fileLines[index]);

	std::string line = _fileLines[index];
	Location loc;
	// _semicolonChecker(line);

	std::vector<std::string> tokens = _split(line);
	std::vector<std::string> insideTokens;
	if (tokens.size() == 2)
	{
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

	int parserIndex;
	bool isEmpty = true;

	LocationFieldParserFuncPtr _location_primitive_parser[NUMBER_OF_LOCATION_PRIMITIVES] = {
		&parser::ConfigParser::_locRootDirParser,
		&parser::ConfigParser::_locAutoIndexParser,
		&parser::ConfigParser::_locIndexParser,
		&parser::ConfigParser::_locAllowedMethodsParser,
		&parser::ConfigParser::_locRedirectionParser,
		&parser::ConfigParser::_locCGIParser,
		&parser::ConfigParser::_locUploadEnableParser,
		&parser::ConfigParser::_locUploadLocationParser,
	};

	index += 2;
	while (_fileLines[index].compare(CLOSING_BRACE))
	{
		if ((parserIndex = _isLocationPrimitive(_fileLines[index])) >= 0)
		{
			isEmpty = false;
			(this->*_location_primitive_parser[parserIndex])(index, loc);
		}
		else if (_fileLines[index].compare(OPENNING_BRACE) && _fileLines[index].compare(CLOSING_BRACE))
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

void parser::ConfigParser::_locRootDirParser(size_t index, Location &loc)
{
	std::string line = _fileLines[index];

	_semicolonChecker(line);

	std::vector<std::string> tokens = _split(line);
	if (tokens.size() == 2)
	{
		loc.setRootDir(tokens[1]);
	}
	else
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
}

void parser::ConfigParser::_locAutoIndexParser(size_t index, Location &loc)
{
	std::string line = _fileLines[index];

	_semicolonChecker(line);

	std::vector<std::string> tokens = _split(line);
	if (tokens.size() == 2)
	{
		if (tokens[1].compare("on") && tokens[1].compare("off"))
			throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
		loc.setAutoIndex((tokens[1] == "on"));
	}
	else
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
}

void parser::ConfigParser::_locIndexParser(size_t index, Location &loc)
{
	std::string line = _fileLines[index];

	_semicolonChecker(line);

	std::vector<std::string> tokens = _split(line);
	if (tokens.size() < 2)
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
	tokens.erase(tokens.begin());
	loc.setDefaultFiles(tokens);
}

void parser::ConfigParser::_locAllowedMethodsParser(size_t index, Location &loc)
{
	std::string line = _fileLines[index];

	_semicolonChecker(line);

	std::vector<std::string> tokens = _split(line);
	std::vector<std::string> insideTokens;

	if (tokens.size() != 2)
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);

	if (std::count(tokens[1].begin(), tokens[1].end(), OPENNING_BRACKET) != 1 ||
		std::count(tokens[1].begin(), tokens[1].end(), CLOSING_BRACKET) != 1 ||
		tokens[1].front() != OPENNING_BRACKET || tokens[1].back() != CLOSING_BRACKET)
		throw std::runtime_error(ERROR_ALLOWED_METHODS_SYNTAX + _fileLines[index]);

	insideTokens = _split(tokens[1], ',');
	if (insideTokens.size() < 1 || insideTokens.size() > 3 ||
		insideTokens.size() != std::count(tokens[1].begin(), tokens[1].end(), ',') + 1)
		throw std::runtime_error(ERROR_ALLOWED_METHODS_SYNTAX + _fileLines[index]);

	loc.setAllowedMethods(insideTokens);
}

void parser::ConfigParser::_locUploadEnableParser(size_t index, Location &loc)
{
	std::string line = _fileLines[index];

	_semicolonChecker(line);

	std::vector<std::string> tokens = _split(line);
	if (tokens.size() == 2)
	{
		if (tokens[1] != "on" && tokens[1] != "off")
			throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
		loc.setUploadEnabled((tokens[1] == "on"));
	}
	else
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
}

void parser::ConfigParser::_locUploadLocationParser(size_t index, Location &loc)
{
	std::string line = _fileLines[index];

	_semicolonChecker(line);

	std::vector<std::string> tokens = _split(line);
	if (tokens.size() == 2)
	{
		loc.setUploadLocation(tokens[1]);
	}
	else
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
}

void parser::ConfigParser::_locRedirectionParser(size_t index, Location &loc)
{
	std::string line = _fileLines[index];

	_semicolonChecker(line);

	std::vector<std::string> tokens = _split(line);
	if (tokens.size() == 3)
	{
		if (!_isSet(tokens[1], &(std::isdigit)))
			throw std::runtime_error(ERROR_ERRPAGE_CODE_NAN);
		loc.setReturnCode(std::atoi(tokens[1].c_str()));
		loc.setReturnUrl(tokens[2]);
	}
	else
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
}

void parser::ConfigParser::_locCGIParser(size_t index, Location &loc)
{
	std::string line = _fileLines[index];

	_semicolonChecker(line);

	std::vector<std::string> tokens = _split(line);
	if (tokens.size() == 2)
	{
		loc.setFastCgiPass(tokens[1]);
	}
	else
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
}
