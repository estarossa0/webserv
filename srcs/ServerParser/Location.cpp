#include "Location.hpp"

parser::Location::Location()
{
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

std::string const & parser::Location::getPath() const
{
	return _path;
}


void parser::Location::setRootDir(std::string const &rd)
{
	_root_dir = rd;
}

std::string const & parser::Location::getRootDir() const
{
	return _root_dir;
}


void parser::Location::setAutoIndex(bool const &ai)
{
	_autoindex = ai;
}

bool const & parser::Location::getAutoIndex() const
{
	return _autoindex;
}


void parser::Location::setDefaultFiles(std::vector<std::string> const &df)
{
	_default_files = df;
}

std::vector<std::string> const & parser::Location::getDefaultFiles() const
{
	return _default_files;
}


void parser::Location::setAllowedMethods(std::vector<std::string> const &am)
{
	_allowed_methods = am;
}

std::vector<std::string> const & parser::Location::getAllowedMethods() const
{
	return _allowed_methods;
}


void parser::Location::setReturnCode(int const &rc)
{
	_return_code = rc;
}

int const & parser::Location::getReturnCode() const
{
	return _return_code;
}


void parser::Location::setReturnUrl(std::string const &ru)
{
	_return_url = ru;
}

std::string const & parser::Location::getReturnUrl() const
{
	return _return_url;
}


void parser::Location::setFastCgiIndex(std::string const &fci)
{
	_fastcgi_index = fci;
}

std::string const & parser::Location::getFastCgiIndex() const
{
	return _fastcgi_index;
}


void parser::Location::setUploadEnabled(bool const &ue)
{
	_upload_enable = ue;
}

bool const & parser::Location::getUploadEnabled() const
{
	return _upload_enable;
}


void parser::Location::setUploadLocation(std::string const &ul)
{
	_upload_location = ul;
}

std::string const & parser::Location::getUploadLocation() const
{
	return _upload_location;
}
