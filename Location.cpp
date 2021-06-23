/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Location.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: renebraaksma <renebraaksma@student.42.f      +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/06/11 10:33:55 by timvancitte   #+#    #+#                 */
/*   Updated: 2021/06/22 15:16:01 by rbraaksm      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"
#include "ConfigParser.hpp"
#include "Error.hpp"

Location::Location(std::string &match) : _autoIndex(false), _ownAutoIndex(false), _ownBodySize(false), _isFileExtension(false), _maxBodySize(1000000)
{
	this->_match = match;
	if (match[0] == '*' && match[1] == '.')
		this->_isFileExtension = true;
	this->_typeFunctionMap.insert(std::make_pair("autoindex", &Location::setAutoIndex));
	this->_typeFunctionMap.insert(std::make_pair("root", &Location::setRoot));
	this->_typeFunctionMap.insert(std::make_pair("method", &Location::setMethod));
	this->_typeFunctionMap.insert(std::make_pair("error_page", &Location::setErrorPage));
	this->_typeFunctionMap.insert(std::make_pair("index", &Location::setIndices));
	this->_typeFunctionMap.insert(std::make_pair("cgi_exec", &Location::setCgiPath));
	this->_typeFunctionMap.insert(std::make_pair("auth_basic", &Location::setAuthBasic));
	this->_typeFunctionMap.insert(std::make_pair("auth_basic_user_file", &Location::setHTPasswordPath));
	this->_typeFunctionMap.insert(std::make_pair("client_max_body_size", &Location::setMaxBodySize));
	return;
}

Location::Location(Location const &src) {
	if (this != &src)
		*this = src;
	return;
}

Location::~Location(void) {
	return;
}

Location&	Location::operator=(Location const &obj)
{
	if (this != &obj) {
		this->_autoIndex = obj._autoIndex;
		this->_match = obj._match;
		this->_root = obj._root;
		this->_methods = obj._methods;
		this->_errorPage = obj._errorPage;
		this->_indices = obj._indices;
		this->_typeFunctionMap = obj._typeFunctionMap;
		this->_authBasic = obj._authBasic;
		this->_authBasicUserFile = obj._authBasicUserFile;
		this->_cgiPath = obj._cgiPath;
		this->_isFileExtension = obj._isFileExtension;
	}
	return *this;
}

void		Location::printLocation() const {
	std::cout << *this << std::endl;
}

void		Location::setAutoIndex(const std::string &autoIndex)
{
	this->_ownAutoIndex = true;
	if (autoIndex == "on")
	{
		this->_autoIndex = true;
		return;
	}
	if (autoIndex != "off")
	{;}
	// return;
}

void		Location::setMaxBodySize(const std::string &maxBodySize)
{
	this->_ownBodySize = true;

	std::stringstream ss;

	ss << maxBodySize;
	ss >> this->_maxBodySize;
	if (this->_maxBodySize == 0)
		this->_maxBodySize = (ULONG_MAX);
	return;
}

void		Location::setRoot(const std::string &root) {
	this->_root = root;
	return;
}

void		Location::setMethod(const std::string &method) {
	std::stringstream ss;
	std::string	methods;

	ss << method;
	while (ss >> methods)
		this->_methods.push_back(methods);
	return;
}

void		Location::setErrorPage(const std::string &errorPage) {
	this->_errorPage = errorPage;
	return;
}

void		Location::setIndices(const std::string &indices) {
	std::stringstream ss;
	std::string	index;

	ss << indices;
	while (ss >> index)
		this->_indices.push_back(index);
	return;
}

void		Location::setCgiPath(const std::string &cgiPath) {
	this->_cgiPath = cgiPath;
	return;
}

void		Location::setAuthBasic(const std::string &authBasic)
{
	this->_authBasic = authBasic;
	return;
}

void		Location::setHTPasswordPath(const std::string &passwordpath) // check if done compiling
{
	struct stat statstruct = {};

	std::fstream configFile;
	std::string line;

	// try
	// {
		if (stat(passwordpath.c_str(), &statstruct) == -1)
		// {
			throw parseError("set password path error", "check input");
			// return;
		// }
		this->_htpasswd_path = passwordpath;

		configFile.open(this->_htpasswd_path.c_str());
		if (!configFile)
		// {
			throw openFileError("Error: failed to open filepath: ", this->_htpasswd_path);
			// return;
		// }
		while (std::getline(configFile, line))
		{
			std::string user;
			std::string pass;
			Utils::getKeyValue(line, user, pass, ":", "\n\r#;");
			this->_loginfo[user] = pass;
		}
		configFile.close();
	// }
	// catch(const std::exception& e)
	// {
		// std::cerr << e.what() << '\n';
		// exit(1);
		// return;
	// }

}


const bool&	Location::hasOwnAutoIndex() const
{
	return this->_ownAutoIndex;
}

const bool&	Location::hasOwnBodySize() const
{
	return this->_ownBodySize;
}

const bool&	Location::getAutoIndex() const {
	return this->_autoIndex;
}

const size_t&	Location::getMaxBodySize() const {
	return this->_maxBodySize;
}

const std::string&	Location::getHTPasswordPath() const {
	return this->_htpasswd_path;
}

const std::string&	Location::getMatch() const {
	return this->_match;
}

const std::string&	Location::getRoot() const {
	return this->_root;
}

const std::string&	Location::getErrorPage() const {
	return this->_errorPage;
}

const std::vector<std::string>&	Location::getIndices() const {
	return this->_indices;
}

const std::vector<std::string>&	Location::getMethods() const {
	return this->_methods;
}

const std::string&	Location::getCGIPath() const {
	return this->_cgiPath;
}

const std::string&	Location::getAuthBasic() const {
	return this->_authBasic;
}

const std::string&	Location::getAuthUserFile() const {
	return this->_authBasicUserFile;
}

const bool&		Location::getIsFileExtension() const {
	return this->_isFileExtension;
}

const std::map<std::string, std::string>&	Location::getLogInfo() const {
	return this->_loginfo;
}
void				Location::findKey(std::string &key, std::string configLine, int lineCount) {
	std::map<std::string, setter>::iterator it;

	std::string parameter;

	if (*(configLine.rbegin()) != ';')
		throw parseError("syntax error, line doesn't end with ';' ", lineCount);
	it = this->_typeFunctionMap.find(key);
	if (it == this->_typeFunctionMap.end())
		throw parseError("key invalid, not found key: " + key + " ", lineCount);
	configLine.resize(configLine.size() - 1); // remove ';'
	parameter = configLine.substr(configLine.find_first_of(WHITESPACE) + 1);
	parameter = Utils::removeLeadingAndTrailingSpaces(parameter);
	(this->*(this->_typeFunctionMap.at(key)))(parameter);
}

// TODO: deze functie kan echt veel mooier
bool			Location::parameterCheck(int &lineCount) const {
	std::vector<std::string>::const_iterator it;

	for (it = this->_methods.begin(); it != this->_methods.end(); ++it)
	{
		if ((*it) != allowedMethods[0] && (*it) != allowedMethods[1] && (*it) != allowedMethods[2] && (*it) != allowedMethods[3] && (*it) != allowedMethods[4])
			throw parseError("invalid method ", lineCount);
	}
	if (this->_root.empty() == true)
		throw parseError("missing root ", lineCount);
	return true;
}

std::ostream&	operator<<(std::ostream &os, const Location &location)
{
	std::vector<std::string> locationMethods;
	std::vector<std::string> locationIndices;
	std::map<std::string, std::string> locationLogInfo;
	locationMethods = location.getMethods();
	locationIndices = location.getIndices();
	locationLogInfo = location.getLogInfo();
	std::vector<std::string>::iterator it_location_methods = locationMethods.begin();
	std::vector<std::string>::iterator it_location_indices = locationIndices.begin();
	std::map<std::string, std::string>::iterator it_location_loginfo = locationLogInfo.begin();


	std::cout << std::setfill('.') << std::left;
	os	<< std::setw(24) << std::left << "location:" << location.getMatch() << '\n'
		<< std::setw(24) << "method:";
	for (;!locationMethods.empty() && it_location_methods != locationMethods.end(); ++it_location_methods) {
		os << *it_location_methods;
	}
	os	<< '\n'
		<< std::setw(24) << std::left << "root:" << location.getRoot() << '\n'
		<< std::setw(24) << std::left << "index:";
	for (;!locationIndices.empty() && it_location_indices!= locationIndices.end(); ++it_location_indices) {
		os << *it_location_indices;
	}
	os	<< '\n'
		<< std::setw(24) << std::left << "auth_basic:" << location.getAuthBasic() << '\n'
		<< std::setw(24) << std::left << "auth_basic_user_file:" << location.getHTPasswordPath() << '\n'
		<< std::setw(24) << std::left << "client_max_body_size:" << location.getMaxBodySize() << '\n'
		<< std::setw(24) << std::left << "cgi_exec:" << location.getCGIPath() << '\n'
		<< std::setw(24) << std::left << "autoindex:" << ((location.getAutoIndex() == true) ? ("on\n") : ("off\n"))
		<< std::setw(24) << std::left << "file_extension:" << ((location.getIsFileExtension() == true) ? ("on\n") : ("off\n"))
		<< std::setw(24) << std::left << "error_page:" << location.getErrorPage() << '\n'
		<< std::setw(24) << std::setfill(' ') << "Log info:" << '\n';
	for (; !locationLogInfo.empty() && it_location_loginfo != locationLogInfo.end(); ++it_location_loginfo)
	{
		os << "User:" << it_location_loginfo->first << "Password:" << it_location_loginfo->second << '\n';
	}
	os << "----------------------------------\n";
	return os;
}