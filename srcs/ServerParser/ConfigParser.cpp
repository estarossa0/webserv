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

parser::ConfigParser::ConfigParser(char const *inFilename) : _filename(inFilename),
															 _serversNumber(0)
{
	_getFileContent();
	std::cout << "got file content successfully" << std::endl;

	_indexServers();
	std::cout << "servers has been indexed successfully" << std::endl;

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
		count = std::count(_fileLines[i].begin(), _fileLines[i].end(), '{') + std::count(_fileLines[i].begin(), _fileLines[i].end(), '}');
		if (count == 0)
			continue;
		else if (count > 1)
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
			}
			else if (_fileLines[start].compare(OPENNING_BRACE) && _fileLines[start].compare(CLOSING_BRACE))
				throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[start]);
			start = static_cast<size_t>(doneParsingIndex);
		}

		i += 2;
	}
}

int parser::ConfigParser::_isPrimitive(std::string const &line)
{
	for (size_t i = 0; i < NUMBER_OF_SERVER_PRIMITIVES; i++)
	{
		if (_fileLines[i].compare(0, primitives_openings[i].size(), primitives_openings[i]) == 0)
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
		if (_fileLines[i].compare(0, location_identifiers[i].size(), location_identifiers[i]) == 0)
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
	int parserIndex;
	int doneParsingIndex;
	bool isEmpty = false;
	LocationFieldParserFuncPtr _location_primitive_parser[NUMBER_OF_SERVER_PRIMITIVES] = {
		
	};

	index += 2;
	Location loc;
	while (_fileLines[index].compare(CLOSING_BRACE))
	{
		if ((parserIndex = _isLocationPrimitive(_fileLines[index])) >= 0)
		{
			(this->*_location_primitive_parser[parserIndex])(index, loc);
			// if ((doneParsingIndex = (this->*_server_primitive_parser[parserIndex])(index, loc)) >= end)
			// 	throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
		}
		else/*  if (_fileLines[index].compare(OPENNING_BRACE) && _fileLines[index].compare(CLOSING_BRACE)) */
			throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
		index++;
	}
	return index + 1;
}

int parser::ConfigParser::_locRootDirParser(size_t index, Location &loc)
{
	std::string line = _fileLines[index];

	_semicolonChecker(line);

	std::vector<std::string> tokens = _split(line);
	if (tokens.size() == 2)
	{
		loc.setRootDir(tokens[1]);
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
	std::string line = _fileLines[index];
	parser::Location loc;

	_semicolonChecker(line);

	std::vector<std::string> tokens = _split(line);
	std::vector<std::string> insideTokens;
	if (tokens.size() == 2)
		loc.setPath(tokens[1]);
	else
		throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
	index += 2;

	std::map<std::string, bool> fields;
	char const *fieldsOp[] = {
		LOC_ROOT,
		LOC_AUTOINDEX,
		LOC_ALLOWED_METHODS,
		LOC_INDEX,
	};

	std::vector<std::string> identifiers(fieldsOp, fieldsOp + 4);
	// verification map initialization
	for (size_t i = 0; i < 4; i++)
		fields.insert(std::pair<std::string, bool>(fieldsOp[i], false));

	while (_fileLines[index].compare(CLOSING_BRACE))
	{
		line = _fileLines[index];
		_semicolonChecker(line);
		tokens = _split(line);
		// root path;
		if (line.compare(0, strlen(fieldsOp[0]), fieldsOp[0]) == 0)
		{
			if (fields[fieldsOp[0]])
				throw std::runtime_error(ERROR_DUPLICATED_FIELD + _fileLines[index]);
			if (tokens.size() > 2)
				throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
			loc.setRootDir(tokens[1]);
			fields[fieldsOp[0]] = true;
		}
		// autoindex (on|off);
		else if (line.compare(0, strlen(fieldsOp[1]), fieldsOp[1]) == 0)
		{
			if (fields[fieldsOp[1]])
				throw std::runtime_error(ERROR_DUPLICATED_FIELD + _fileLines[index]);
			if (tokens.size() > 2 || tokens[1].compare("on") || tokens[1].compare("off"))
				throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
			loc.setAutoIndex((tokens[1] == "on"));
			fields[fieldsOp[1]] = true;
		}
		// allowed methods
		else if (line.compare(0, strlen(fieldsOp[2]), fieldsOp[2]) == 0)
		{
			if (fields[fieldsOp[2]])
				throw std::runtime_error(ERROR_DUPLICATED_FIELD + _fileLines[index]);
			if (std::count(line.begin(), line.end(), '[') != 1 ||
				std::count(line.begin(), line.end(), ']') != 1)
				throw std::runtime_error(ERROR_ALLOWED_METHODS_SYNTAX + _fileLines[index]);
			if (tokens.size() == 2)
			{
				if (tokens[1].front() == OPENNING_BRACKET && tokens[1].back() == CLOSING_BRACKET)
				{
					insideTokens = _split(tokens[1], ',');
					if (insideTokens.size() == std::count(tokens[1].begin(), tokens[1].end(), ',') + 1)
					{
						loc.setAllowedMethods(insideTokens);
						fields[fieldsOp[2]] = true;
					}
					else
						throw std::runtime_error(ERROR_ALLOWED_METHODS_SYNTAX + _fileLines[index]);
				}
				else
					throw std::runtime_error(ERROR_ALLOWED_METHODS_SYNTAX + _fileLines[index]);
			}
			else
				throw std::runtime_error(ERROR_ALLOWED_METHODS_SYNTAX + _fileLines[index]);
		}
		// files paths
		else if (line.compare(0, strlen(fieldsOp[3]), fieldsOp[3]) == 0)
		{
			if (fields[fieldsOp[3]])
				throw std::runtime_error(ERROR_DUPLICATED_FIELD + _fileLines[index]);
			if (tokens.size() < 2)
				throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);
			tokens.erase(tokens.begin());
			loc.setDefaultFiles(tokens);
			fields[fieldsOp[3]] = true;
		}
		// anything else is invalid
		else
			throw std::runtime_error(ERROR_INVALID_CONFIGURATION + _fileLines[index]);

		index++;
	}

	for (size_t i = 0; i < 4; i++)
	{
		if (fields[fieldsOp[i]])
			break;
		if (i + 1 == 4)
			throw std::runtime_error(ERROR_EMPTY_LOCATION_CONFIG);
	}

	_checked_primitives[LOCATION_OP] = true;
	return index + 1;
}
