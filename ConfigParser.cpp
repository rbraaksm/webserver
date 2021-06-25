/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ConfigParser.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: renebraaksma <renebraaksma@student.42.f      +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/06/09 15:10:54 by timvancitte   #+#    #+#                 */
/*   Updated: 2021/06/25 10:28:13 by rbraaksm      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

ConfigParser::ConfigParser(char **argv) :_argv(argv) {
	return;
}

ConfigParser::ConfigParser(ConfigParser const &src) {
	if (this != &src)
		*this = src;
	return;
}

ConfigParser::~ConfigParser() {
	return;
}

ConfigParser&	ConfigParser::operator=(ConfigParser const &obj)
{
	if (this != &obj) {
		this->_argv = obj._argv;
	}
	return *this;
}

std::fstream&	ConfigParser::getConfigFile()
{
	return this->_configFile;
}

void	ConfigParser::openConfigFile()
{
	this->_configFile.open(this->_argv[1]);
	if (!this->_configFile)	{
		throw openFileError("Error: failed to open filepath: ", this->_argv[1]);
	}
}

void	ConfigParser::removeComments(std::string *line)
{
	line->erase(std::find(line->begin(), line->end(), '#'), line->end());
}

bool	ConfigParser::receiveNextLine(std::string *line, size_t *count)
{
	++(*count);
	std::getline(this->_configFile, (*line));
	return true;
}

void	ConfigParser::setLocation(Server *newServer)
{
	std::string locationPath = Utils::checkLocationPath(*this->_it, *this->_count_it);
	Location *newLocation = new Location(locationPath);

	this->plusIterators();
	while (*this->_it != "}")
	{
		if (*this->_it == "server {" || Utils::findFirstWord(*this->_it) == "location")
			throw parseError("location block isn't closed ", *this->_count_it);
		std::string key = Utils::findFirstWord(*this->_it);
		if (key.size() <= 0)
			throw parseError("not found " + *this->_it, *this->_count_it);
		newLocation->findKey(key, *this->_it, *this->_count_it);
		this->plusIterators();
	}
	this->plusIterators();
	newLocation->parameterCheck(*this->_count_it);
	newServer->addLocation(newLocation);
}

void	ConfigParser::createServer(ServerCluster *serverCluster)
{
	Server	*newServer = new Server;
	this->plusIterators();
	while(*this->_it != "}")
	{
		if (Utils::findFirstWord(*_it) == "location")
			this->setLocation(newServer);
		else
		{
			std::string key = Utils::findFirstWord(*this->_it);
			newServer->findKey(key, *this->_it, *this->_count_it);
			this->plusIterators();
		}
	}
	newServer->setAutoIndexOfLocations();
	newServer->parameterCheck(*this->_count_it);
	serverCluster->addServer(newServer);
	// std::cout << *newServer << std::endl;
}

void	ConfigParser::plusIterators(void)
{
	++this->_count_it;
	++this->_it;
}

void	ConfigParser::lookingForServer(ServerCluster *serverCluster)
{
	this->_count_it = _configCount.begin();
	for(this->_it = this->_configLines.begin(); this->_it != this->_configLines.end(); ++this->_it)
	{
		if (*this->_it != "server {")
			throw parseError("no Server block detected", *this->_count_it);
		this->createServer(serverCluster);
		++this->_count_it;
	}
}

bool	ConfigParser::createLine(std::string *line, size_t *count)
{
	int i = 0;
	while (receiveNextLine(line, count))
	{
		if (i > 0)
			return false;
		if (this->_configFile.eof())
			++i;
		this->removeComments(line);
		if (Utils::skipEmptyLine(line) == true)
			continue;
		Utils::removeSpacesBeforeAfter(line);
		break ;
	}
	return true;
}

void	ConfigParser::createArray(void)
{
	size_t open = 0;
	size_t closed = 0;
	size_t count = 0;
	std::string	line;
	while (this->createLine(&line, &count))
	{
		this->_configLines.push_back(line);
		this->_configCount.push_back(count);
		open += std::count(line.begin(), line.end(), '{');
		closed += std::count(line.begin(), line.end(), '}');
		if (open < closed)
			throw parseError("Brackets aren't open ", count);
	}
	if (open > closed)
		throw parseError("Brackets aren't closed ", count);
}

void	ConfigParser::printFile(void)
{
	std::cout << "Print file:" << std::endl;
	this->_count_it = _configCount.begin();
	for (this->_it = _configLines.begin(); this->_it != _configLines.end(); ++this->_it)
	{
		std::cout << "[" << *this->_count_it << "] " << *this->_it << std::endl;
		++this->_count_it;
	}
}

void	ConfigParser::parseTheConfigFile(ServerCluster *serverCluster)
{
	this->createArray();
	// this->printFile();

	this->lookingForServer(serverCluster);
	if (serverCluster->clusterIsEmpty())
		throw clusterError("Cluster seems to be empty", "check your input");
	_configCount.clear();
	_configLines.clear();
	(void)serverCluster;
}

