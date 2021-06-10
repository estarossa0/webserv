#include "Location.hpp"

char const *parser::Location::standard_allowed_methods[3] = {
	"GET",
	"POST",
	"DELETE",
};

parser::Location::Location() : _isRedirection(false),
							   _isCGI(false)
{
	for (size_t i = 0; i < 3; i++)
		_allowed_methods.insert(std::pair<std::string, bool>(standard_allowed_methods[i], false));
}

parser::Location::~Location()
{
	_default_files.clear();
	_allowed_methods.clear();
}

void parser::Location::setPath(std::string const &p)
{
	_path = p;
}

std::string const &parser::Location::getPath() const
{
	return _path;
}

void parser::Location::setRootDir(std::string const &rd)
{
	_root_dir = rd;
}

std::string const &parser::Location::getRootDir() const
{
	return _root_dir;
}

void parser::Location::setAutoIndex(bool const &ai)
{
	_autoindex = ai;
}

bool const &parser::Location::getAutoIndex() const
{
	return _autoindex;
}

void parser::Location::setDefaultFiles(std::vector<std::string> const &df)
{
	_default_files = df;
}

std::vector<std::string> const &parser::Location::getDefaultFiles() const
{
	return _default_files;
}

void parser::Location::setAllowedMethods(std::vector<std::string> const &am)
{
	size_t i;
	for (size_t j = 0; j < am.size(); j++)
	{

		for (i = 0; i < 3; i++)
		{
			if (am[j].compare(standard_allowed_methods[i]))
				break;
		}
		if (i == 3)
			throw std::invalid_argument(am[j] + " is not among standard allowed methods which are only GET, POST and DELETE");
		if (_allowed_methods[am[j]] == true)
			throw std::invalid_argument("duplicated allowed method: " + am[j]);
		_allowed_methods[am[j]] = true;
	}
}

std::map<std::string, bool> const &parser::Location::getAllowedMethods() const
{
	return _allowed_methods;
}

void parser::Location::setReturnCode(int const &rc)
{
	_isRedirection = true;
	_return_code = rc;
}

int const &parser::Location::getReturnCode() const
{
	return _return_code;
}

void parser::Location::setReturnUrl(std::string const &ru)
{
	_isRedirection = true;
	_return_url = ru;
}

std::string const &parser::Location::getReturnUrl() const
{
	return _return_url;
}

bool const &parser::Location::isRedirection() const
{
	return _isRedirection;
}

void parser::Location::setFastCgiPass(std::string const &fci)
{
	_fastcgi_pass = fci;
}

std::string const &parser::Location::getFastCgiPass() const
{
	return _fastcgi_pass;
}

void parser::Location::setUploadEnabled(bool const &ue)
{
	_upload_enable = ue;
}

bool const &parser::Location::getUploadEnabled() const
{
	return _upload_enable;
}

void parser::Location::setIsCGI(bool const &iscgi)
{
	_isCGI = iscgi;
}

bool const &parser::Location::isCGI() const
{
	return _isCGI;
}

void parser::Location::setUploadLocation(std::string const &ul)
{
	_upload_location = ul;
}

std::string const &parser::Location::getUploadLocation() const
{
	return _upload_location;
}
