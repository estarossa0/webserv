#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <vector>


namespace parser
{
	class Location
	{
	private:
		std::string _path;
		std::string _root_dir;
		bool _autoindex;
		std::vector<std::string> _default_files;
		std::vector<std::string> _allowed_methods;
		int _return_code;
		std::string _return_url;
		std::string _fastcgi_index;
		bool _upload_enable;
		std::string _upload_location;



	public:
		Location();
		~Location();

		void setPath(std::string const &);
		std::string const & getPath() const;

		void setRootDir(std::string const &);
		std::string const & getRootDir() const;
		
		void setAutoIndex(bool const &);
		bool const & getAutoIndex() const;
		
		void setDefaultFiles(std::vector<std::string> const &);
		std::vector<std::string> const & getDefaultFiles() const;
		
		void setAllowedMethods(std::vector<std::string> const &);
		std::vector<std::string> const & getAllowedMethods() const;
		
		void setReturnCode(int const &);
		int const & getReturnCode() const;

		void setReturnUrl(std::string const &);
		std::string const & getReturnUrl() const;

		void setFastCgiIndex(std::string const &);
		std::string const & getFastCgiIndex() const;

		void setUploadEnabled(bool const &);
		bool const & getUploadEnabled() const;

		void setUploadLocation(std::string const &);
		std::string const & getUploadLocation() const;
	};


} // namespace parser

#endif // !LOCATION_HPP 