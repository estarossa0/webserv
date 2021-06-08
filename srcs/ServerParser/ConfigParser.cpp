#include "ConfigParser.hpp"

std::string const parser::ConfigParser::primitives_openings[NUMBER_OF_SERVER_PRIMITIVES] = {
	PORT_OP,
	HOST_OP,
	SERVER_NAME_OP,
	CLIENT_MAX_SIZE_BODY_OP,
	ERROR_PAGE_OP,
	ROOT_OP,
	LOCATION_OP,
	RETURN_LOCATION_OP,
	CGI_LOCATION_OP,
	UPLOAD_LOCATION_OP,
};

parser::ConfigParser::ConfigParser(char const *inFilename) : _filename(inFilename),
															 _serversNumber(0)
{
	_getFileContent();
	std::cout << "got file content successfully" << std::endl;
	_indexServers();
	std::cout << "servers has been indexed successfully" << std::endl;
	_initParsersFuntions();
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
}

void parser::ConfigParser::_parseContent()
{
	size_t start;
	size_t end;

	size_t i = 0;
	while (i < _serversIndexing.size())
	{
		start = _serversIndexing[i] + 1;
		end = _serversIndexing[i + 1];
		if (start >= end)
			throw std::runtime_error(ERROR_EMPTY_CONFIGURATION);
		Server sv;
		while(start < end)
		{
			for (size_t j = 0; j < NUMBER_OF_SERVER_PRIMITIVES; i++)
			{
				if (_fileLines[i].compare(primitives_openings[j]))
				{
					
				}
			}
		}
		
		


		i += 2;
	}
}

void parser::ConfigParser::_initParsersFuntions()
{
	for (size_t i = 0; i < NUMBER_OF_SERVER_PRIMITIVES ; i++)
	{
		_checked_primitives.insert(std::pair<std::string, bool>(primitives_openings[i], false));
	}

	// _server_primitive_parser[0] = 
}