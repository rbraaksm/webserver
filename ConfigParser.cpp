/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ConfigParser.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: renebraaksma <renebraaksma@student.42.f      +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/06/09 15:10:54 by timvancitte   #+#    #+#                 */
/*   Updated: 2021/06/24 13:08:21 by rbraaksm      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

ConfigParser::ConfigParser(int argc, char **argv) : _argc(argc), _argv(argv) {
	this->_lineCount = 0;
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

ConfigParser&		ConfigParser::operator=(ConfigParser const &obj)
{
	if (this != &obj) {
		this->_argc = obj._argc;
		this->_argv = obj._argv;
	}
	return *this;
}

std::fstream&	ConfigParser::getConfigFile()
{
	return this->_configFile;
}

void		ConfigParser::openConfigFile()
{
	this->_configFile.open(this->_argv[1]);
	if (!this->_configFile)	{
		throw openFileError("Error: failed to open filepath: ", this->_argv[1]);
	}
}

void		ConfigParser::removeComments()
{
	this->_configLine.erase(std::find(this->_configLine.begin(), this->_configLine.end(), '#'), this->_configLine.end());
}

bool		ConfigParser::receiveNextLine()
{
	++this->_lineCount;
	std::getline(this->_configFile, this->_configLine);
	return true;
}

void		ConfigParser::setLocation(Server *newServer)
{
	std::string locationPath = Utils::checkLocationPath(*this->_it, this->_lineCount);
	Location *newLocation = new Location(locationPath);

	++this->_it;
	while (*this->_it != "}")
	{
		if (*this->_it == "server {" || Utils::findFirstWord(*this->_it) == "location")
			throw parseError("block not closed ", this->_lineCount);
		std::string key = Utils::findFirstWord(*this->_it);
		if (key.size() <= 0)
			throw parseError("not found " + *this->_it, this->_lineCount);
		newLocation->findKey(key, *this->_it, this->_lineCount);
		++this->_it;
	}
	++this->_it;
	newLocation->parameterCheck(this->_lineCount);
	newServer->addLocation(newLocation);
}

void		ConfigParser::createServer(ServerCluster *serverCluster)
{
	Server	*newServer = new Server;
	++this->_it;
	while(*this->_it != "}")
	{
		if (Utils::findFirstWord(*_it) == "location")
			this->setLocation(newServer);
		else
		{
			std::string key = Utils::findFirstWord(*this->_it);
			newServer->findKey(key, *this->_it, this->_lineCount);
			++this->_it;
		}
	}
	newServer->setAutoIndexOfLocations();
	newServer->parameterCheck(this->_lineCount);
	serverCluster->addServer(newServer);
	std::cout << *newServer << std::endl;
}


void		ConfigParser::lookingForServer(ServerCluster *serverCluster)
{
	for(this->_it = this->_configLines.begin(); this->_it != this->_configLines.end();)
	{
		if (*this->_it == "server {")
			this->createServer(serverCluster);
		++this->_it;
	}
}

bool		ConfigParser::createLine()
{
	int i = 0;
	while (receiveNextLine())
	{
		if (i > 0)
			return false;
		if (this->_configFile.eof())
			++i;
		this->removeComments();
		if (Utils::skipEmptyLine(this->_configLine) == true)
			continue;
		this->_configLine = Utils::removeLeadingAndTrailingSpaces(this->_configLine);
		break ;
	}
	return true;
}

void		ConfigParser::createArray(void)
{
	size_t open = 0;
	size_t closed = 0;
	while (this->createLine())
	{
		this->_configLines.push_back(this->_configLine);
		open += std::count(this->_configLine.begin(), this->_configLine.end(), '{');
		closed += std::count(this->_configLine.begin(), this->_configLine.end(), '}');
		if (open < closed)
			throw parseError("Brackets aren't open ", this->_lineCount);
	}
	if (open > closed)
		throw parseError("Brackets aren't closed ", this->_lineCount);
}

void		ConfigParser::printFile(void)
{
	std::cout << "Print file:" << std::endl;
	for (this->_it = _configLines.begin(); this->_it != _configLines.end(); ++this->_it)
		std::cout << *this->_it << std::endl;
}

void		ConfigParser::parseTheConfigFile(ServerCluster *serverCluster)
{
	this->createArray();
	// this->printFile();

	this->lookingForServer(serverCluster);
	if (serverCluster->clusterIsEmpty())
		throw clusterError("Cluster seems to be empty", "check your input");
}

