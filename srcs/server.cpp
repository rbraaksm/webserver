#include "../includes/server.hpp"
#include "../includes/manipulateUtils.hpp"
#include "../includes/parseUtils.hpp"
#include "../includes/getInfoUtils.hpp"
#include "../includes/error.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

Server::Server() :
	_port(0),
	_tcpSocket(0),
	_addr_len(0)
	{
		_typeFunctionMap.insert(std::make_pair("port", &Server::configurePort));
		_typeFunctionMap.insert(std::make_pair("host", &Server::configureHost));
		_typeFunctionMap.insert(std::make_pair("error_page", &Server::configureErrorPage));
		_typeFunctionMap.insert(std::make_pair("server_name", &Server::configureServerName));
		_typeFunctionMap.insert(std::make_pair("time_out", &Server::configureTimeOut));
		_typeFunctionMap.insert(std::make_pair("cgi_file_types", &Server::configureCgiFileTypes));
	}

Server::~Server(){}


void	Server::createServer(std::map<int, std::string>::iterator &it)
{
	locationContext	location;
	std::string 		key;

	resetServer();
	for (; it->second.compare("}") != 0; ++it)
	{
		key = getInfoUtils::findFirstWord(it->second);
		if (parseUtils::isServerKey(key) == true)
		{
			if (key.compare("location") == 0)
			{
				location.setLocation(it);
				_locationBlocks.push_back(location);
			}
			else
				(this->*(this->_typeFunctionMap.at(key)))(getInfoUtils::createParameter(it->second));
		}
		else
			throw parseError("key", key, it->first);
	}
}

void	Server::configurePort(const std::string& str) {
	_port = atoi(str.c_str());
}

void	Server::configureHost(const std::string& str) {
	_host = str;
}

void	Server::configureServerName(const std::string& str) {
	_serverName = str;
}

void	Server::configureErrorPage(const std::string& str) {
	_errorPage = str;
}

void	Server::configureTimeOut(const std::string& str) {
	_timeOut = atoi(str.c_str());
}

void	Server::configureCgiFileTypes(const std::string& str) {
	_cgiFileTypes = str;
}

void	Server::createSocket()
{
	int option = 1;
	if ((this->_tcpSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw (std::runtime_error("Socket creation failed"));
	if (setsockopt(this->_tcpSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1)
		throw(std::runtime_error("SETSOCKOPT failed"));
	if (fcntl(this->_tcpSocket, F_SETFL, O_NONBLOCK) == -1)
		throw (std::runtime_error("FCNTL failed"));
}

void	Server::bindSocketAddress(int port)
{
	memset((char *)&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_addr.sin_port = htons(port);
	if (bind(this->_tcpSocket, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
		throw (std::runtime_error("Bind failed"));
}

void	Server::createConnection(int backlog)
{
	if (listen(this->_tcpSocket, backlog) == -1)
		throw (std::runtime_error("Listen failed"));
}

void	Server::resetServer()
{
	_port = 0;
	_timeOut = 0;
	_host.clear();
	_serverName.clear();
	_errorPage.clear();
	_cgiFileTypes.clear();
	_locationBlocks.clear();
}

void	Server::removeHandledRequest(int usedFD){
	std::map<int, requestBuf>::iterator request = _requestBuffer.find(usedFD);

	request->second.getBody().clear();
	request->second.setBodySize(0);
	_requestBuffer.erase(request);
}

void	Server::removeClient(int clientFD)
{
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		int client;
		if (clientFD == (client = it->getClientFD()))
		{
			close(client);
			_clients.erase(it);
			return ;
		}
	}
}

int	Server::updateRequestBuffer(int fd, const std::string& requestData) {
	std::map<int, requestBuf>::iterator it = _requestBuffer.find(fd);

	if (it == _requestBuffer.end()) {
		requestBuf tmp;
		_requestBuffer.insert(std::pair<int, requestBuf>(fd, tmp));
	}

	if (!_requestBuffer[fd].headersReceived()) {
		_requestBuffer[fd].addHeaderData(requestData);
	}
	else {
		_requestBuffer[fd].addBodyData(requestData);
	}

	if (!_requestBuffer[fd].headersReceived())
		return INVALID;
	else
		return _requestBuffer[fd].validateRequest();
}

int	Server::getTimeOut() {
	return _timeOut;
}

int	Server::getPort() {
	return _port;
}

int	Server::getTcpSocket() {
	return _tcpSocket;
}

std::string	Server::getServerName() {
	return _serverName;
}

std::string	Server::getErrorPage() {
	return _errorPage;
}