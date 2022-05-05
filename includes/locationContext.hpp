#ifndef LOCATIONCONTEXT_HPP
# define LOCATIONCONTEXT_HPP

# include <iostream>
# include <vector>
# include <map>

enum	location_values
{
	ERROR_CODE = 400,
	UNUSED = -1
};

class locationContext
{
	private:
		typedef void	(locationContext::*setter)(const std::string&);
		std::map<std::string, std::string>	_userAndPassword;
		std::map<std::string, setter>		_typeFunctionMap;
		std::map<int, std::string>			_return;
		std::vector<std::string>			_allowedMethod;

		std::string	_locationContext;
		std::string	_root;
		std::string	_index;
		std::string	_authBasic;

		int			_maxFileSize;
		bool		_autoIndex;
		bool		_alias;

	public:
		locationContext();
		~locationContext();

		void	cleanLocationInstance();
		void	configureRoot(const std::string &str);
		void	configureAllowedMethod(const std::string &str);
		void	configureIndex(const std::string &str);
		void	configureAutoIndex(const std::string &str);
		void	configureAlias(const std::string &str);
		void	configureMaxFileSize(const std::string &str);
		void	configureAuthBasic(const std::string &str);
		void	configureAuthUserInfo(const std::string &str);
		void	configureReturn(const std::string &str);
		void	setLocation(std::map<int, std::string>::iterator &it);

		std::map<int, std::string>			getReturn();
		std::map<std::string, std::string>	getUserAndPassword();
		std::vector<std::string>			getMethod();
		std::string							getLocationContext();
		std::string							getRoot();
		std::string							getIndex();
		std::string							getAuthBasic();
		bool								getAutoIndex();
		bool								getAlias();
		int									getMaxFileSize();
		
};

#endif
