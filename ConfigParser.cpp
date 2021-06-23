/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ConfigParser.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: renebraaksma <renebraaksma@student.42.f      +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/06/09 15:10:54 by timvancitte   #+#    #+#                 */
/*   Updated: 2021/06/22 15:43:46 by rbraaksm      ########   odam.nl         */
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
	std::string configLine;
	std::string locationPath;

	locationPath = Utils::checkLocationPath(startline, this->_lineCount);
	Location *newLocation = new Location(locationPath);
	while (std::getline(this->_configFile, configLine))
	{
		++this->_lineCount;
		if (this->_configFile.eof())
			throw parseError("end of file ", this->_lineCount);
		if (Utils::skipEmptyLine(configLine))
			continue;
		if (configLine[0] == '#')
			continue;
		configLine = Utils::removeLeadingAndTrailingSpaces(configLine);

		if (configLine == "server {" || Utils::findFirstWord(configLine) == "location")
		{
			throw parseError("block not closed ", this->_lineCount);
			return NULL;
		}
		if (configLine == "}")
			break;
		std::string key = Utils::findFirstWord(configLine);
		if (key.size() <= 0)
			throw parseError("not found " + configLine, this->_lineCount);
		newLocation->findKey(key, configLine, this->_lineCount); // removed try/catch block
	}
	newLocation->parameterCheck(this->_lineCount);
	if (this->_configFile.eof())
		throw parseError("end of file ", this->_lineCount);
	return (newLocation);
}

void		ConfigParser::removeComments(std::string configLine)
{
	configLine.erase(std::find(configLine.begin(), configLine.end(), '#'), configLine.end()); // remove comments
}


void		ConfigParser::parseLoop(ServerCluster *serverCluster, std::string configLine, std::string Block)
{
	while (std::getline(this->_configFile, configLine))
	{
		++this->_lineCount;
		if (this->_configFile.eof())
			break;
		this->removeComments(configLine);
		if (Utils::skipEmptyLine(configLine) == true)
			continue;
		configLine = Utils::removeLeadingAndTrailingSpaces(configLine);
		if (configLine[0] == '#')
			continue;


		if (configLine != "server {")
			throw parseError("no Server block detected", this->_lineCount);
		Server	*newServer = new Server;
		this->_serverBlockIsOpen = 1;
		while (std::getline(this->_configFile, configLine))
		{
			this->_lineCount++;
			if (this->_configFile.eof())
				throw parseError("end of file ", this->_lineCount);
			configLine.erase(std::find(configLine.begin(), configLine.end(), '#'), configLine.end()); // remove comments
			if (Utils::skipEmptyLine(configLine) == true)
				continue;
			if (configLine[0] == '#')
				continue;
			configLine = Utils::removeLeadingAndTrailingSpaces(configLine);
			if (Utils::findFirstWord(configLine) == "location")
			{
				Location *newLocation = getLocation(configLine);
				newServer->addLocation(newLocation);
			}
			else if (configLine == "}")
			{
				this->_serverBlockIsOpen = 0;
				break;
			}
			else {
				std::string key = Utils::findFirstWord(configLine);
				// if (key.size() <= 0)
				// 	throw parseError("not found " + configLine, this->_lineCount);
				newServer->findKey(key, configLine, this->_lineCount); // removed try/catch block
			}
		}
		newServer->setAutoIndexOfLocations();
		newServer->parameterCheck(this->_lineCount);
		serverCluster->addServer(newServer);
		std::cout << *newServer << std::endl;
		(void)Block;
	}
	if (this->_serverBlockIsOpen)
		throw parseError("Server block isn't closed ", ++this->_lineCount);
}


void		ConfigParser::parseTheConfigFile(ServerCluster *serverCluster)
{
	std::string configLine;

	while(1)
	{
		/* .eof() break */
		/* Looking for server { */
		/*		-->   while loop init serverBlock */
		
		/* looking for } --> back to first loop */
		break ;
	}

	this->parseLoop(serverCluster, configLine, "Server");
	if (serverCluster->clusterIsEmpty())
		throw clusterError("Cluster seems to be empty", "check your input");
}

