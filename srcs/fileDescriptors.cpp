#include "../includes/fileDescriptors.hpp"

fileDescriptors::fileDescriptors() : _max(-1){
	FD_ZERO(&_read);
	FD_ZERO(&_write);
	FD_ZERO(&_readBuffer);
	FD_ZERO(&_writeBuffer);
}

fileDescriptors::~fileDescriptors() {
	_timeOutMonitor.clear();
}

void	fileDescriptors::synchronize(std::vector<Server> &servers) {
	_read = _readBuffer;
	_write = _writeBuffer;

	for (size_t index = 0; index < servers.size(); index++) {
		FD_SET(servers[index].getTcpSocket(), &_read);
	}
}

void	fileDescriptors::initializeMax(std::vector<Server> servers) {
	for (size_t index = 0; index < servers.size(); index++) {
		if (servers[index].getTcpSocket() > _max)
			_max = servers[index].getTcpSocket();
	}
}

void	fileDescriptors::updateMax(int fd) {
	if (_max > fd)
		return;
	_max = fd;
}

void	fileDescriptors::acceptedRequestUpdate(int activeFD) {
	setReadBuffer(activeFD);
	updateMax(activeFD);
}

void	fileDescriptors::handledRequestUpdate(int fileFD, int activeFD, std::string cgiFileTypes, std::string contentType, std::string method) {
	clearFromReadBuffer(activeFD);
	updateMax(fileFD);
	if (fileFD != -1)
		setReadBuffer(fileFD);
	else
		setWriteBuffer(activeFD);

	if (cgiFileTypes.find(contentType) != std::string::npos || method == "PUT" || method == "POST")
		setWriteBuffer(fileFD);
}

void	fileDescriptors::readRequestUpdate(int fileFD, int activeFD) {
	if (readyForWriting(fileFD))
		clearFromWriteBuffer(fileFD);
	clearFromReadBuffer(fileFD);
	setWriteBuffer(activeFD);
}

void	fileDescriptors::sendResponseUpdate(Client &clientCurrent) {
	clearFromWriteBuffer(clientCurrent._clientFD);
	if (clientCurrent._cgiInputFD != -1)
		close(clientCurrent._cgiInputFD);
	clientCurrent._cgiInputFD = -1;
	close(clientCurrent._fileFD);
	clientCurrent._fileFD = -1;
	setReadBuffer(clientCurrent._clientFD);
}

void    fileDescriptors::syncMaxFD(std::vector<Server> &servers) {
	int maxFD = 0;

	for (size_t serverIndex = 0; serverIndex < servers.size(); serverIndex++) {
		Server *server = &servers[serverIndex];
		for(size_t clientIndex = 0;  clientIndex < server->_clients.size(); clientIndex++) {
			Client *currentClient = &server->_clients[clientIndex];
			if (maxFD < currentClient->getClientFD())
				maxFD = currentClient->getClientFD();
			if (maxFD < currentClient->getFileFD())
				maxFD = currentClient->getFileFD();
			if (maxFD < currentClient->getCgiInputFD())
				maxFD = currentClient->getCgiInputFD();
		}
		if (maxFD < server->getTcpSocket())
			maxFD = server->getTcpSocket();
	}
	_max = maxFD + 1;
}

void 	fileDescriptors::updateActiveClient(std::vector<Client> &clients, int clientFD) {
	bool activate = false;
	bool restartCycle = true;

	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
		int fd = it->getClientFD();
		it->setActive(false);
		if (fd == clientFD) {
			activate = true;
			continue;
		}
		if (activate == true) {
			it->setActive(true);
			activate = false;
			restartCycle = false;
		}
	}
	if (restartCycle == true)
		clients.begin()->setActive(true);
}

void	fileDescriptors::setReadBuffer(int fd) {
	FD_SET(fd, &_readBuffer);
}

void	fileDescriptors::setWriteBuffer(int fd) {
	FD_SET(fd, &_writeBuffer);
}

void	fileDescriptors::clearFromReadBuffer(int fd) {
	FD_CLR(fd, &_readBuffer);
}

void	fileDescriptors::clearFromWriteBuffer(int fd) {
	FD_CLR(fd, &_writeBuffer);
}

void	fileDescriptors::clearFDSets(void)
{
	FD_ZERO(&_readBuffer);
	FD_ZERO(&_writeBuffer);
}

int	fileDescriptors::readyForReading(int fd) {
	if (fd == -1)
	{
		close(fd);
		return 0;
	}
	return FD_ISSET(fd, &_read);
}

int	fileDescriptors::readyForWriting(int fd) {
	if (fd == -1)
	{
		close(fd);
		return 0;
	}
	return FD_ISSET(fd, &_write);
}

void	fileDescriptors::setTimeOut(int fd) {
	struct timeval	timeval;
	long long		time;
	std::map<int, long long>::iterator	it = _timeOutMonitor.find(fd);

	gettimeofday(&timeval, NULL);
	time = timeval.tv_sec;

	if (it == _timeOutMonitor.end())
		_timeOutMonitor.insert(std::pair<int, long long>(fd, time));
	else
		it->second = time;
}

void	fileDescriptors::checkTimeOut(std::vector<Client> &clients, int clientFD, int timeOut) {
	struct timeval	timeval;
	long long		time;

	gettimeofday(&timeval, NULL);
	time = timeval.tv_sec;

	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++) {
		int	fd = it->getClientFD();
		if (fd == clientFD)
		{
			std::map<int, long long>::iterator recordedTime = _timeOutMonitor.find(clientFD);
			if (recordedTime->second != 0 && (time - recordedTime->second) >= timeOut) {
				clearFromReadBuffer(clientFD);
				close(clientFD);
				clients.erase(it);
			}
			return ;
		}
	}
}

fd_set&	fileDescriptors::getRead() {
	return _read;
}

fd_set&	fileDescriptors::getWrite() {
	return _write;
}

int		fileDescriptors::getMax() {
	return _max + 1;
}
