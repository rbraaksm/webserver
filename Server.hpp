/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: renebraaksma <renebraaksma@student.42.f      +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/06/10 13:57:16 by timvancitte   #+#    #+#                 */
/*   Updated: 2021/06/22 13:32:38 by rbraaksm      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "Webserver.hpp"
#include "Error.hpp"
#include "Location.hpp"
// #include "Connection.hpp"
#include <fstream>

class Server
{
	public:

		typedef void	(Server::*setter)(std::string&);

	private:

		int 							_portNumber;
		size_t 							_maxBodySize;
		bool 							_autoIndex;
		std::string						_root;
		std::string						_errorPage;
		std::string						_host;
		std::vector<std::string>		_serverNames;
		std::vector<std::string>		_indices;
		std::vector<Location*>			_locations;
		std::vector<Server*>			_alternativeServers;
		std::map<std::string, setter>	_typeFunctionMap;

		long _socketFD;
		struct sockaddr_in _addr;

	public:

		size_t _bodylen;
		// Connection connections[NR_OF_CONNECTIONS];
		Server(void);
		Server(Server const &src);
		~Server(void);

		Server&		operator=(Server const &obj);

		void	setPortNumber(std::string &listen);
		void	setMaxBodySize(std::string &maxBodySize);
		void	setAutoIndex(std::string &autoIndex);
		void	setRoot(std::string &root);
		void	setErrorPage(std::string &errorPage);
		void	setHost(std::string &host);
		void	setServerNames(std::string &serverNames);
		void	setIndices(std::string &indices);
		void	setAlternativeServers(Server *alternative);

		void	setupRespStr(int index);

		const int						&getPortNumber() const;
		const size_t					&getMaxBodySize() const;
		const bool						&getAutoIndex() const;
		const std::string				&getRoot() const;
		const std::string				&getErrorPage() const;
		const std::string				&getHost() const;
		const std::vector<std::string>	&getServerNames() const;
		const std::vector<std::string>	&getIndices() const;
		const std::vector<Location*>	&getLocations() const;

		const long						&getSocketFD() const;
		const struct sockaddr_in		&getSocketAddress() const;

		void							findKey(std::string &key, std::string configLine, int lineCount);
		bool							parameterCheck(int &lineCount) const;
		void							addLocation(Location *newLocation);
		void							setAutoIndexOfLocations();
		void							startListening();

		Location*						findLocation(std::string &match);

		// void							createResponse(int index);

};

std::ostream&	operator<<(std::ostream &os, const Server &server);

#endif
