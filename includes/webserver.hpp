#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include "client.hpp"
# include "server.hpp"
# include "fileDescriptors.hpp"

# define RESET		"\033[0m"
# define RED		"\033[31m"
# define GREEN		"\033[32m"
# define YEL		"\033[33m"

enum	webserver_values
{
	WBUNUSED = -1,
	ERROR = 400
};

class Server;
class fileDescriptors;

class Webserver
{
	private:
		std::vector<Server>	_servers;
		bool				_timeOutCheck;
		int					_responseCount;

	public:
		Webserver();
		~Webserver();

		void	lookingForServer(std::map<int, std::string> lines);
		void	parseConfiguration(std::string path);
		void	establishConnection();
		void	initializeForRun();
		void	run(fileDescriptors& fd);
		void	serverLoop(fileDescriptors& fd);
		bool	acceptRequest(fileDescriptors& fd, Server** server);
		void	handleRequest(fileDescriptors& fd, Client** clientCurrent, Server** server);
		bool	sendResponse(fileDescriptors& fd, Client** clientCurrent, Server** server, int client_index);
		bool	writeRequestContentToFile(fileDescriptors& fd, Client** clientCurrent, Server** server);
		bool	readFullRequestFromFile(Client** clientCurrent);
		void	clientLoop(fileDescriptors& fd, Server* server);
		void	setTimeOutCheck(Server* server, Client *clientCurrent);
		void	cleanFD(fileDescriptors &fd, int fdToClean, int checkUnused);
		void	cleanClient(const Client *clientCurrent, Server* server);
		void	cleanUpAfterError(fileDescriptors &fd, Server* server, const Client *clientCurrent, const std::string &e) ;
};

#endif