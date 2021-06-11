#include "Location.hpp"

char const *Location::standard_allowed_methods[3] = {
	"GET",
	"POST",
	"DELETE",
};

Location::Location() : _isRedirection(false),
							   _isCGI(false),
							   _upload_enable(false)
{
	for (size_t i = 0; i < 3; i++)
		_allowed_methods.insert(std::pair<std::string, bool>(standard_allowed_methods[i], false));
}

Location::~Location()
{
	_default_files.clear();
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

void Location::setDefaultFiles(std::vector<std::string> const &df)
{
	_default_files = df;
}

std::vector<std::string> const &Location::getDefaultFiles() const
{
	return _default_files;
}

void Location::setAllowedMethods(std::vector<std::string> const &am)
{
	size_t i;
	for (size_t j = 0; j < am.size(); j++)
	{

		for (i = 0; i < 3; i++)
		{
			if (am[j] == standard_allowed_methods[i])
				break;
		}
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
	out << "\n================ Location ===============" << std::endl;
	out << "location path : [" << loc.getPath() << "]" << std::endl;
	out << "location root : [" << loc.getRootDir() << "]" << std::endl;
	out << "location autoindex : [" << loc.getAutoIndex() << "]" << std::endl;
	
	out << "location indexes : ";
	std::vector<std::string> defaultFiles = loc.getDefaultFiles();
	for (size_t i = 0; i < defaultFiles.size(); i++)
	{
		out <<"[" << defaultFiles[i] << "]  ";	
	}
	out << std::endl;

	std::map<std::string, bool> map(loc.getAllowedMethods());
	for (std::map<std::string, bool>::iterator it = map.begin(); it != map.end(); it++)
	{
		out << "location method: [" << it->first << "] = [" << it->second << "]" << std::endl;
	}
	
	out << "location is redirection : " << loc.isRedirection() << std::endl;
	out << "location return, code : [" << loc.getReturnCode() << "], url: [" << loc.getReturnUrl() << "]" << std::endl;
	out << "location is CGI : " << loc.isCGI() << std::endl;
	out << "location CGI, url: [" << loc.getFastCgiPass() << "]" << std::endl;
	out << "location upload enabled : " << loc.getUploadEnabled() << std::endl;
	out << "location Upload store: [" << loc.getUploadLocation() << "]" << std::endl;

	out << "============== Location End =============\n";
	return out;
}
