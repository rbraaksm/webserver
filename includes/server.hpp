#ifndef SERVER_HPP
# define SERVER_HPP

# include "locationContext.hpp"
# include "requestBuf.hpp"
# include "client.hpp"
# include <netinet/in.h>

class Client;

class Server
{
	public:
		friend class Webserver;
		friend class fileDescriptors;

	private:
		typedef void					(Server::*setter)(const std::string&);
		std::map<std::string, setter>	_typeFunctionMap;
		std::map<int, requestBuf>		_requestBuffer;
		std::vector<locationContext>	_locationBlocks;
		std::vector<Client>				_clients;
		struct sockaddr_in				_addr;
		int								_port;
		int								_timeOut;
		int								_tcpSocket;
		int								_addr_len;
		std::string						_host;
		std::string						_serverName;
		std::string						_errorPage;
		std::string						_cgiFileTypes;

	public:
		Server();
		~Server();

		void	configurePort(const std::string& str);
		void	configureHost(const std::string& str);
		void	configureServerName(const std::string& str);
		void	configureTimeOut(const std::string& str);
		void	configureCgiFileTypes(const std::string& str);
		void	configureErrorPage(const std::string& str);
		void	createSocket();
		void	bindSocketAddress(int port);
		void	createConnection(int backlog);
		void	resetServer();
		void	removeHandledRequest(int usedFD);
		void	removeClient(int clientFD);
		void	createServer(std::map<int, std::string>::iterator &it);

		int updateRequestBuffer(int fd, const std::string& requestData);
		int getTimeOut();
		int getPort();
		int getTcpSocket();

		std::string getServerName();
		std::string getErrorPage();
};

#endif
