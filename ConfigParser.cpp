/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ConfigParser.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: renebraaksma <renebraaksma@student.42.f      +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/06/09 15:10:54 by timvancitte   #+#    #+#                 */
/*   Updated: 2021/06/23 11:22:33 by rbraaksm      ########   odam.nl         */
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

Location*	ConfigParser::getLocation(std::string &startline)
{
	std::string locationPath;

	locationPath = Utils::checkLocationPath(startline, this->_lineCount);
	Location *newLocation = new Location(locationPath);
	while (1)
	{
		if (this->skipEmptyLineAndComments() == false)
				throw parseError("end of file ", this->_lineCount);

		if (this->_configLine == "server {" || Utils::findFirstWord(this->_configLine) == "location")
		{
			throw parseError("block not closed ", this->_lineCount);
			return NULL;
		}
		if (this->_configLine == "}")
			break;
		std::string key = Utils::findFirstWord(this->_configLine);
		if (key.size() <= 0)
			throw parseError("not found " + this->_configLine, this->_lineCount);
		newLocation->findKey(key, this->_configLine, this->_lineCount);
	}
	newLocation->parameterCheck(this->_lineCount);
	if (this->_configFile.eof())
		throw parseError("end of file ", this->_lineCount);
	return (newLocation);
}

void		ConfigParser::removeComments(void)
{
	this->_configLine.erase(std::find(this->_configLine.begin(), this->_configLine.end(), '#'), this->_configLine.end()); // remove comments
}

bool		ConfigParser::receiveNextLine(void)
{
	++this->_lineCount;
	std::getline(this->_configFile, this->_configLine);
	return true;
}

bool		ConfigParser::skipEmptyLineAndComments(void)
{
	while (receiveNextLine())
	{
		if (this->_configFile.eof())
			return false;
		this->removeComments();
		if (Utils::skipEmptyLine(this->_configLine) == true)
			continue;
		this->_configLine = Utils::removeLeadingAndTrailingSpaces(this->_configLine);
		if (this->_configLine[0] == '#')
			continue;
		break ;		
	}
	return true;
}

void		ConfigParser::setLocation(Server *newServer)
{
	Location *newLocation = getLocation(this->_configLine);
	newServer->addLocation(newLocation);
}

void		ConfigParser::createServer(ServerCluster *serverCluster)
{
		Server	*newServer = new Server;
		this->_serverBlockIsOpen = 1;
		while(1)
		{
			if (this->skipEmptyLineAndComments() == false)
				throw parseError("end of file ", this->_lineCount);
	
			if (Utils::findFirstWord(this->_configLine) == "location")
				this->setLocation(newServer);
			else if (this->_configLine == "}")
			{
				this->_serverBlockIsOpen = 0;
				break;
			}
			else {
				std::string key = Utils::findFirstWord(this->_configLine);
				newServer->findKey(key, this->_configLine, this->_lineCount);
			}
		}
		newServer->setAutoIndexOfLocations();
		newServer->parameterCheck(this->_lineCount);
		serverCluster->addServer(newServer);
		std::cout << *newServer << std::endl;
}
void		ConfigParser::lookingForServer(ServerCluster *serverCluster)
{
		if (this->skipEmptyLineAndComments() == true)
		{
			if (this->_configLine != "server {")
				throw parseError("no Server block detected", this->_lineCount);
			this->createServer(serverCluster);
			this->lookingForServer(serverCluster);
		}
	if (this->_serverBlockIsOpen)
		throw parseError("Server block isn't closed ", ++this->_lineCount);

}


void		ConfigParser::parseTheConfigFile(ServerCluster *serverCluster)
{
	this->lookingForServer(serverCluster);
	if (serverCluster->clusterIsEmpty())
		throw clusterError("Cluster seems to be empty", "check your input");
}

