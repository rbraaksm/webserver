#ifndef FILEDESCRIPTORS_HPP
# define FILEDESCRIPTORS_HPP

#include <sys/time.h>
#include <sys/socket.h>
#include <vector>
#include <unistd.h>
#include "server.hpp"
#include "client.hpp"

class fileDescriptors {
	public:
		friend class Webserver;

	private:
		std::map<int, long long>	_timeOutMonitor;
		fd_set	_read;
		fd_set	_write;
		fd_set	_readBuffer;
		fd_set	_writeBuffer;
		int		_max;

	public:
		fileDescriptors();
		~fileDescriptors();
		void	synchronize(std::vector<Server> &servers);
		void	initializeMax(std::vector<Server> servers);
		void	updateMax(int fd);
		void	acceptedRequestUpdate(int activeFD);
		void	handledRequestUpdate(int fileFD, int activeFD, std::string cgiFileTypes, std::string contentType, std::string method);
		void	readRequestUpdate(int fileFD, int activeFD);
		void	sendResponseUpdate(Client &clientCurrent);
		void	syncMaxFD(std::vector<Server> &servers);
		void	updateActiveClient(std::vector<Client> &clients, int clientFD);
		void	setReadBuffer(int fd);
		void	setWriteBuffer(int fd);
		void	clearFromReadBuffer(int fd);
		void	clearFromWriteBuffer(int fd);
		void	clearFDSets(void);
		void	setTimeOut(int fd);
		void	checkTimeOut(std::vector<Client> &clients, int clientFD, int timeOut);
		fd_set&	getRead();
		fd_set&	getWrite();
		int		readyForReading(int fd);
		int		readyForWriting(int fd);
		int		getMax();
};

#endif
