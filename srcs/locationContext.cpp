#include "../includes/locationContext.hpp"
#include "../includes/parseUtils.hpp"
#include "../includes/fileUtils.hpp"
#include "../includes/getInfoUtils.hpp"
#include "../includes/error.hpp"
#include <fcntl.h>
#include <fstream>

locationContext::locationContext() :
	_maxFileSize(0),
	_autoIndex(false),
	_alias(false)
	{
		_typeFunctionMap.insert(std::make_pair("root", &locationContext::configureRoot));
		_typeFunctionMap.insert(std::make_pair("allowed_method", &locationContext::configureAllowedMethod));
		_typeFunctionMap.insert(std::make_pair("autoindex", &locationContext::configureAutoIndex));
		_typeFunctionMap.insert(std::make_pair("index", &locationContext::configureIndex));
		_typeFunctionMap.insert(std::make_pair("alias", &locationContext::configureAlias));
		_typeFunctionMap.insert(std::make_pair("max_file_size", &locationContext::configureMaxFileSize));
		_typeFunctionMap.insert(std::make_pair("auth_basic", &locationContext::configureAuthBasic));
		_typeFunctionMap.insert(std::make_pair("auth_user_file", &locationContext::configureAuthUserInfo));
		_typeFunctionMap.insert(std::make_pair("return", &locationContext::configureReturn));
	}

locationContext::~locationContext(){}

void	locationContext::setLocation(std::map<int, std::string>::iterator &it)
{
	std::string key;

	cleanLocationInstance();
	getInfoUtils::setLocationContext(it->second, _locationContext);
	++it;
	for (; it->second.compare("}") != 0; ++it)
	{
		key = getInfoUtils::findFirstWord(it->second);
		if (parseUtils::isLocationKey(key) == true)
		{
			(this->*(this->_typeFunctionMap.at(key)))(it->second);
			if (!_allowedMethod.empty())
				parseUtils::isMethodAllowed(_allowedMethod, it->first);
		}
		else
			throw parseError("key", key, it->first);
	}
}

void	locationContext::cleanLocationInstance()
{
	_locationContext.clear();
	_root.clear();
	_index.clear();
	_allowedMethod.clear();
	_maxFileSize = 0;
	_autoIndex = false;
	_alias = false;
	_authBasic.clear();
}

void	locationContext::configureRoot(const std::string &str){
	_root = getInfoUtils::getSingleWordFromString(str);
}

void	locationContext::configureAllowedMethod(const std::string &str){
	_allowedMethod = getInfoUtils::getWordsFromString(str);
}

void	locationContext::configureIndex(const std::string &str){
	_index = getInfoUtils::getSingleWordFromString(str);
}

void	locationContext::configureMaxFileSize(const std::string &str) {
	_maxFileSize = getInfoUtils::getNumberFromString(str);
}

void	locationContext::configureAuthBasic(const std::string &str){
	_authBasic = getInfoUtils::getSingleWordFromString(str);
}

void	locationContext::configureAuthUserInfo(const std::string &str)
{
	std::fstream	fd;
	std::string		line;

	fileUtils::openFile(fd, getInfoUtils::getSingleWordFromString(str));
	while (std::getline(fd, line))
	{
		std::string user = getInfoUtils::getUser(line);
		_userAndPassword.insert(std::make_pair<std::string, std::string>(user, line));
	}
	fileUtils::closeFile(fd);
	fd.close();
}

void	locationContext::configureAutoIndex(const std::string &str){
	if (getInfoUtils::getSingleWordFromString(str) == "on")
		_autoIndex = true;
}


void	locationContext::configureAlias(const std::string &str){
	if (getInfoUtils::getSingleWordFromString(str) == "on")
		_alias = true;
}

void	locationContext::configureReturn(const std::string &str)
{
	int redirectCode = getInfoUtils::getNumberFromString(str);
	if (redirectCode < 300 || redirectCode > 310)
		throw std::runtime_error("Wrong redirection code");

	size_t statusCodeStart = str.find_first_of('3');
	size_t pos = str.find_first_of(' ', statusCodeStart);
	std::string newLocation = str.substr(pos + 1);

	_return.insert(std::pair<int, std::string>(redirectCode, newLocation));
}

std::map<std::string, std::string>	locationContext::getUserAndPassword() {
	return _userAndPassword;
}

std::map<int, std::string>	locationContext::getReturn() {
	return _return;
}

std::vector<std::string>	locationContext::getMethod() {
	return _allowedMethod;
}

std::string	locationContext::getLocationContext() {
	return _locationContext;
}

std::string	locationContext::getRoot() {
	return _root;
}

std::string	locationContext::getIndex() {
	return _index;
}

std::string	locationContext::getAuthBasic() {
	return _authBasic;
}

bool	locationContext::getAutoIndex() {
	return _autoIndex;
}

bool	locationContext::getAlias() {
	return _alias;
}

int	locationContext::getMaxFileSize() {
	return _maxFileSize;
}

