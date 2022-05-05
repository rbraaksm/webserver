#include "../includes/webserver.hpp"
#include "../includes/parseUtils.hpp"
#include "../includes/getInfoUtils.hpp"
#include "../includes/headerUtils.hpp"
#include "../includes/error.hpp"
#include <fcntl.h>
#include <sys/select.h>
#include <unistd.h>

Webserver::Webserver() : _timeOutCheck(false), _responseCount(0) {
}

Webserver::~Webserver(){
}

void	Webserver::lookingForServer(std::map<int, std::string> lines)
{
	Server	_server;
	std::map<int, std::string>::iterator	it = lines.begin();

	for (; it != lines.end(); ++it)
	{
		if (it->second != "server {")
			throw parseError("no serverblock detected", it->first);
		_server.createServer(++it);
		_servers.push_back(_server);
	}
	parseUtils::checkIfServersAreValid(_servers);
}

void	Webserver::parseConfiguration(std::string path)
{
	// std::string path = file;
	std::map<int, std::string>	lines;

	parseUtils::getInfoFromConfigFile(lines, path);
	lookingForServer(lines);
	
}

void	Webserver::establishConnection()
{
	for (size_t index = 0; index < _servers.size(); index++)
	{
		_servers[index].createSocket();
		_servers[index].bindSocketAddress(_servers[index].getPort());
		_servers[index].createConnection(1000);
	}
}

void	Webserver::initializeForRun() {
	fileDescriptors	fd;
	fd.initializeMax(_servers);
	run(fd);
}

void	Webserver::run(fileDescriptors& fd) {
	std::cout << YEL << "--- Server is running ---" << RESET << std::endl;
	while (true)
	{
		fd.synchronize(this->_servers);
		if (select(fd.getMax(), &fd.getRead(), &fd.getWrite(), 0, 0) == -1) {
			fd.clearFDSets();
			continue ;
		}
		serverLoop(fd);
		fd.syncMaxFD(this->_servers);
	} // WHILE(TRUE) LOOP
}

void	Webserver::serverLoop(fileDescriptors& fd) {
	for (size_t serverIndex = 0; serverIndex < _servers.size(); serverIndex++) {
		Server *server = &_servers[serverIndex];
		if (acceptRequest(fd, &server) == false)
			continue;
		clientLoop(fd, server);
	} // FOR LOOP SERVER
}

bool Webserver::acceptRequest(fileDescriptors& fd, Server** server) {
	if (fd.readyForReading((*server)->getTcpSocket())) // accept request
	{
		int newFD = accept((*server)->getTcpSocket(), (struct sockaddr *) &(*server)->_addr, (socklen_t *) &(*server)->_addr_len);
		if (newFD == -1)
		{
			close(newFD);
			return false ;
		}
		if (fcntl(newFD, F_SETFL, O_NONBLOCK) == -1)
			return false ;
		fd.setTimeOut(newFD);
		fd.acceptedRequestUpdate(newFD);
		(*server)->_clients.push_back(Client(newFD));
		if ((*server)->_clients.size() == 1)
			(*server)->_clients.begin()->_active = true;
	}
	return true ;
}

void	Webserver::handleRequest(fileDescriptors& fd, Client** clientCurrent, Server** server) {
	std::string requestHeaders;
	getInfoUtils::readBrowserRequest(requestHeaders, (*clientCurrent)->_clientFD);
	if (!requestHeaders.empty()) {
		this->_timeOutCheck = false;
		fd.setTimeOut((*clientCurrent)->_clientFD);
	}
	if (!requestHeaders.empty() && (*server)->updateRequestBuffer((*clientCurrent)->_clientFD, requestHeaders) == VALID) {
		(*clientCurrent)->_handler.parseRequest((*server)->_requestBuffer[(*clientCurrent)->_clientFD]);
		(*server)->removeHandledRequest((*clientCurrent)->_clientFD);
		(*clientCurrent)->_fileFD = (*clientCurrent)->_handler.handleRequest((*server)->_cgiFileTypes, (*server)->_locationBlocks, (*server)->getErrorPage(), &(*clientCurrent)->_authorizationStatus);
		fd.handledRequestUpdate((*clientCurrent)->_fileFD, (*clientCurrent)->_clientFD, (*server)->_cgiFileTypes, (*clientCurrent)->_handler.verifyContentType(), (*clientCurrent)->_handler.getMethod());
		if ((*server)->_cgiFileTypes.find((*clientCurrent)->_handler.verifyContentType()) != std::string::npos) {
			(*clientCurrent)->_cgiInputFD = (*clientCurrent)->_handler.createCgiFD("input");
			fd.setWriteBuffer((*clientCurrent)->_cgiInputFD);
			fd.updateMax((*clientCurrent)->_cgiInputFD);
		}
	}
}

static void	printLine(int clients, int current, int count)
{
	std::cout << "Active clients [" << GREEN << clients << RESET << "]";
	std::cout << " amount of clients [" << GREEN << current << RESET << "]";
	std::cout << " response number [" << GREEN << count << "]" << RESET << std::endl;
}

bool	Webserver::sendResponse(fileDescriptors& fd, Client** clientCurrent, Server** server,int clientIndex) {
	this->_timeOutCheck = false;
	if (!(*clientCurrent)->_handler.getBytesWritten())
		(*clientCurrent)->_handler.createResponse((*clientCurrent)->_fileFD, (*server)->_serverName);
	(*clientCurrent)->_handler.sendResponse((*clientCurrent)->_clientFD);
	if ((*clientCurrent)->_handler.getBytesWritten() < (int)(*clientCurrent)->_handler.getResponseSize())
		return false;
	(*clientCurrent)->_handler.setBytesWritten(0);
	fd.sendResponseUpdate((*server)->_clients[clientIndex]);
	fd.updateActiveClient((*server)->_clients, (*clientCurrent)->_clientFD);
	printLine((*server)->_clients.size(), (*clientCurrent)->_clientFD, this->_responseCount);
	this->_responseCount++;
	return true;
}

bool	Webserver::writeRequestContentToFile(fileDescriptors& fd, Client** clientCurrent, Server** server) {
	if (fd.readyForWriting((*clientCurrent)->_fileFD) && (*clientCurrent)->_handler.getStatus() < ERROR && !(*clientCurrent)->_handler.getWriteToFile()) {
		if ((*clientCurrent)->_handler.getBytesWritten() < (int)(*clientCurrent)->_handler.getBody().size() || (int)(*clientCurrent)->_handler.getBody().size() == 0) {
			if ((*server)->_cgiFileTypes.find((*clientCurrent)->_handler.verifyContentType()) != std::string::npos && fd.readyForWriting((*clientCurrent)->_cgiInputFD))
				(*clientCurrent)->_handler.executeCgi((*clientCurrent)->_cgiInputFD, (*clientCurrent)->_fileFD, (*server)->_serverName, (*server)->_port, (*clientCurrent)->_authorizationStatus, (*clientCurrent)->_handler.getAuthorization());
			else
				(*clientCurrent)->_handler.writeBodyToFile((*clientCurrent)->_fileFD);
			if ((int)(*clientCurrent)->_handler.getBody().size() == 0)
				(*clientCurrent)->_handler.setWriteToFile(true);
			return false;
		}
		(*clientCurrent)->_handler.setBytesWritten(0);
		(*clientCurrent)->_handler.setWriteToFile(true);
		this->_timeOutCheck = false;
	}
	return true;
}

bool	Webserver::readFullRequestFromFile(Client** clientCurrent) {
	if ((*clientCurrent)->_handler.getStatus() != 204 && !(*clientCurrent)->_handler.getReadFromFile()) {
		int ret = 0;
		if ((*clientCurrent)->_handler.verifyContentType() == "bla" && (*clientCurrent)->_handler.getMethod() == "POST")
			ret = (*clientCurrent)->_handler.readCgiOutputFile((*clientCurrent)->_fileFD, (int)(*clientCurrent)->_handler.getBody().size());
		else
			ret = (*clientCurrent)->_handler.readRequestedFile((*clientCurrent)->_fileFD);
		if (ret)
			return false;
		(*clientCurrent)->_handler.setBytesRead(0);
		(*clientCurrent)->_handler.setReadFromFile(true);
		this->_timeOutCheck = false;
	}
	return true;
}

void	Webserver::clientLoop(fileDescriptors& fd, Server* server) {
	for (size_t clientIndex = 0;  clientIndex < server->_clients.size(); clientIndex++) {
		Client *clientCurrent = &server->_clients[clientIndex];
		try {
			setTimeOutCheck(server, clientCurrent);
			if (!clientCurrent->_active)
				continue;
			if (fd.readyForReading(clientCurrent->_clientFD)) //handle requested file
				handleRequest(fd, &clientCurrent, &server);
			if (fd.readyForReading(clientCurrent->_fileFD)) //read requested file
			{
				if (writeRequestContentToFile(fd, &clientCurrent, &server) == false)
					continue;
				if (readFullRequestFromFile(&clientCurrent) == false)
					continue;
				fd.readRequestUpdate(clientCurrent->_fileFD, clientCurrent->_clientFD);
				if (clientCurrent->_cgiInputFD != WBUNUSED)
					fd.clearFromWriteBuffer(clientCurrent->_cgiInputFD);
			}
			if (fd.readyForWriting(clientCurrent->_clientFD)) //send response
				if (sendResponse(fd, &clientCurrent, &server, clientIndex) == false)
					continue ;
			if (this->_timeOutCheck) {
				fd.updateActiveClient(server->_clients, clientCurrent->_clientFD);
				fd.checkTimeOut(server->_clients, clientCurrent->getClientFD(), server->_timeOut);
			}
		} //TRY BLOCK
		catch (std::string &e) {
			cleanUpAfterError(fd, server, clientCurrent, e);
		}
	} // FOR LOOP CLIENT
}

void Webserver::setTimeOutCheck(Server* server, Client *clientCurrent) {
	this->_timeOutCheck = !(
			(server->_requestBuffer.find(clientCurrent->_clientFD) != server->_requestBuffer.end() &&
			 server->_requestBuffer[clientCurrent->_clientFD].getBodySize() != 0)
			|| clientCurrent->_handler.getBytesWritten());
}

void	Webserver::cleanFD(fileDescriptors &fd, int fdToClean, int checkUnused) {
	if (fd.readyForReading(fdToClean))
		fd.clearFromReadBuffer(fdToClean);
	if (fd.readyForWriting(fdToClean))
		fd.clearFromWriteBuffer(fdToClean);
	if (checkUnused == 1 && fdToClean != WBUNUSED)
		close(fdToClean);
}

void	Webserver::cleanClient(const Client *clientCurrent, Server *server) {
	if (server->_clients.size() == 1) {
		close(clientCurrent->_clientFD);
		server->_clients.clear();
	}
	else
		server->removeClient(clientCurrent->_clientFD);
}

void Webserver::cleanUpAfterError(fileDescriptors &fd, Server* server, const Client *clientCurrent, const std::string &e) {
	cleanFD(fd, clientCurrent->_clientFD, 0);
	cleanFD(fd, clientCurrent->_fileFD, 1);
	cleanFD(fd, clientCurrent->_cgiInputFD, 1);
	cleanClient(clientCurrent, server);
	std::cout << RED << e << RESET << std::endl;
}
