/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ConfigParser.hpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: renebraaksma <renebraaksma@student.42.f      +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/06/09 14:50:15 by timvancitte   #+#    #+#                 */
/*   Updated: 2021/06/23 18:04:58 by rbraaksm      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include "Webserver.hpp"
#include "Error.hpp"
#include "ParserUtils.hpp"
#include "Server.hpp"
#include "Location.hpp"
#include "ServerCluster.hpp"

class ConfigParser
{
	private:

		std::fstream 	_configFile;
		std::string		_configLine;
		int				_argc;
		char			**_argv;
		int 			_lineCount;
		int				_serverBlockIsOpen;
		size_t			_open;
		size_t			_closed;
		ConfigParser(void);

	public:

		ConfigParser(int argc, char **argv);
		ConfigParser(ConfigParser const &src);
		~ConfigParser(void);

		ConfigParser&		operator=(ConfigParser const &obj);


		void			parseTheConfigFile(ServerCluster *serverCluster);
		Location*		getLocation(std::string &startline);
		void			openConfigFile();
		std::fstream&	getConfigFile();
		static int				getLineCount();

		void			removeComments();
		void			getServerBlock(ServerCluster *serverCluster, std::string configLine);
		void			lookingForServer(ServerCluster *serverCluster);

		bool			receiveNextLine();
		bool			skipEmptyLineAndComments();
		void			createServer(ServerCluster *serverCluster);
		void			setLocation(Server *newServer);

		void			globalScope();

};

#endif