#include "Webserv.hpp"

char const *Location::standard_allowed_methods[3] = {
	"GET",
	"POST",
	"DELETE",
};

Location::Location() : _path("/"),
					   _isRedirection(false),
					   _isCGI(false),
					   _upload_enable(false)
{
	for (size_t i = 0; i < 3; i++)
		_allowed_methods.insert(std::pair<std::string, bool>(standard_allowed_methods[i], false));
	_allowed_methods["GET"] = true;
}

Location::Location(Location const &rhs)
{
	*this = rhs;
}

Location &Location::operator=(Location const &rhs)
{
	if (this != &rhs)
	{
		_path = rhs._path;
		_root_dir = rhs._root_dir;
		_autoindex = rhs._autoindex;
		_default_file = rhs._default_file;
		_allowed_methods = rhs._allowed_methods;
		_return_code = rhs._return_code;
		_return_url = rhs._return_url;
		_isRedirection = rhs._isRedirection;
		_fastcgi_pass = rhs._fastcgi_pass;
		_isCGI = rhs._isCGI;
		_upload_enable = rhs._upload_enable;
		_upload_location = rhs._upload_location;
	}
	return *this;
}

Location::~Location()
{
	_allowed_methods.clear();
}

void Location::setPath(std::string const &p)
{
	_path = p;
}

std::string const &Location::getPath() const
{
	return _path;
}

void Location::setRootDir(std::string const &rd)
{
	_root_dir = rd;
}

std::string const &Location::getRootDir() const
{
	return _root_dir;
}

void Location::setAutoIndex(bool const &ai)
{
	_autoindex = ai;
}

bool const &Location::getAutoIndex() const
{
	return _autoindex;
}

void Location::setDefaultFile(std::string const &df)
{
	_default_file = df;
}

std::string const &Location::getDefaultFile() const
{
	return _default_file;
}

void Location::setAllowedMethods(std::vector<std::string> const &am)
{
	size_t i;
	for (size_t i = 0; i < 3; i++)
		_allowed_methods[standard_allowed_methods[i]] = false;
	for (size_t j = 0; j < am.size(); j++)
	{
		for (i = 0; i < 3; i++)
			if (am[j] == standard_allowed_methods[i])
				break;
		if (i == 3)
			throw std::invalid_argument(am[j] + " is not among standard allowed methods which are only GET, POST and DELETE");
		if (_allowed_methods[am[j]] == true)
			throw std::invalid_argument("duplicated allowed method: " + am[j]);
		_allowed_methods[am[j]] = true;
	}
}

std::map<std::string, bool> const &Location::getAllowedMethods() const
{
	return _allowed_methods;
}

void Location::setReturnCode(int const &rc)
{
	_isRedirection = true;
	_return_code = rc;
}

int const &Location::getReturnCode() const
{
	return _return_code;
}

void Location::setReturnUrl(std::string const &ru)
{
	_isRedirection = true;
	_return_url = ru;
}

std::string const &Location::getReturnUrl() const
{
	return _return_url;
}

bool const &Location::isRedirection() const
{
	return _isRedirection;
}

void Location::setFastCgiPass(std::string const &fci)
{
	_fastcgi_pass = fci;
}

std::string const &Location::getFastCgiPass() const
{
	return _fastcgi_pass;
}

void Location::setUploadEnabled(bool const &ue)
{
	_upload_enable = ue;
}

bool const &Location::getUploadEnabled() const
{
	return _upload_enable;
}

void Location::setIsCGI(bool const &iscgi)
{
	_isCGI = iscgi;
}

bool const &Location::isCGI() const
{
	return _isCGI;
}

void Location::setUploadLocation(std::string const &ul)
{
	_upload_location = ul;
}

std::string const &Location::getUploadLocation() const
{
	return _upload_location;
}

std::ostream &operator<<(std::ostream &out, const Location &loc)
{
	out << std::boolalpha;
	out << "\n================ Location [" << loc.getPath() << "]===============" << std::endl;
	if (!loc.getRootDir().empty())
		out << " root : [" << loc.getRootDir() << "]" << std::endl;
	out << " autoindex : [" << loc.getAutoIndex() << "]" << std::endl;
	if (!loc.getDefaultFile().empty())
		out << " index : [" << loc.getDefaultFile() << "]" << std::endl;
	std::map<std::string, bool> map(loc.getAllowedMethods());
	out << " allowed methods:";
	for (std::map<std::string, bool>::iterator it = map.begin(); it != map.end(); it++)
	{
		if (map[it->first])
			out << " [ " << it->first << " ]  ";
	}
	out << std::endl;
	if (loc.isRedirection())
	{
		out << " Is redirection : " << loc.isRedirection() << std::endl;
		out << " return, code : [" << loc.getReturnCode() << "], url: [" << loc.getReturnUrl() << "]" << std::endl;
	}
	if (loc.isCGI())
	{
		out << " Is CGI : " << loc.isCGI() << std::endl;
		out << " CGI path: [" << loc.getFastCgiPass() << "]" << std::endl;
	}
	out << " upload enabled : " << loc.getUploadEnabled() << std::endl;
	if (!loc.getUploadLocation().empty())
		out << " upload store: [" << loc.getUploadLocation() << "]" << std::endl;
	out << "============== Location End [" << loc.getPath() << "]===============\n";
	return out;
}
