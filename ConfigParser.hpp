/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ConfigParser.hpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: renebraaksma <renebraaksma@student.42.f      +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/06/09 14:50:15 by timvancitte   #+#    #+#                 */
/*   Updated: 2021/06/24 17:04:45 by rbraaksm      ########   odam.nl         */
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

		std::fstream 						_configFile;
		std::vector<std::string>			_configLines;
		std::vector<std::string>::iterator	_it;

		std::vector<int>					_configCount;
		std::vector<int>::iterator			_count_it;
		char								**_argv;
		ConfigParser(void);

	public:

		ConfigParser(char **argv);
		ConfigParser(ConfigParser const &src);
		~ConfigParser(void);

		ConfigParser&		operator=(ConfigParser const &obj);


		void			parseTheConfigFile(ServerCluster *serverCluster);
		Location*		getLocation(std::string &startline);
		void			openConfigFile();
		std::fstream&	getConfigFile();
		static int		getLineCount();

		void			getServerBlock(ServerCluster *serverCluster, std::string configLine);
		void			lookingForServer(ServerCluster *serverCluster);

		void			createServer(ServerCluster *serverCluster);
		void			setLocation(Server *newServer);

		bool			receiveNextLine(std::string *line, size_t *count);
		void			removeComments(std::string *line);
		bool			createLine(std::string *line, size_t *count);
		void			createArray();

		void			printFile();
		void			plusIterators();

};

#endif