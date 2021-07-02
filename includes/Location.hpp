#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "Webserv.hpp"

class Location
{
private:
	std::string _path;
	std::string _root_dir;
	bool _autoindex;
	std::string _default_file;
	std::map<std::string, bool> _allowed_methods;
	int _return_code;
	std::string _return_url;
	bool _isRedirection;
	std::string _fastcgi_pass;
	bool _isCGI;
	bool _upload_enable;
	std::string _upload_location;

public:
	Location();
	Location(Location const &);
	Location &operator=(Location const &);
	~Location();

	void setPath(std::string const &);
	std::string const &getPath() const;

	void setRootDir(std::string const &);
	std::string const &getRootDir() const;

	void setAutoIndex(bool const &);
	bool const &getAutoIndex() const;

	void setDefaultFile(std::string const &);
	std::string const &getDefaultFile() const;

	void setAllowedMethods(std::vector<std::string> const &);
	std::map<std::string, bool> const &getAllowedMethods() const;

	void setReturnCode(int const &);
	int const &getReturnCode() const;

	void setReturnUrl(std::string const &);
	std::string const &getReturnUrl() const;
	bool const &isRedirection() const;

	void setFastCgiPass(std::string const &);
	std::string const &getFastCgiPass() const;

	void setIsCGI(bool const &);
	bool const &isCGI() const;

	void setUploadEnabled(bool const &);
	bool const &getUploadEnabled() const;

	void setUploadLocation(std::string const &);
	std::string const &getUploadLocation() const;

	static char const *standard_allowed_methods[3];
};

std::ostream &operator<<(std::ostream &, const Location &);

#endif // !LOCATION_HPP
