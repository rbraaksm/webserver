/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ConfigParser.hpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: renebraaksma <renebraaksma@student.42.f      +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/06/09 14:50:15 by timvancitte   #+#    #+#                 */
/*   Updated: 2021/06/22 15:36:13 by rbraaksm      ########   odam.nl         */
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

		std::fstream _configFile;
		int	_argc;
		char **_argv;
		int _lineCount;
		int	_serverBlockIsOpen;
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

		void			removeComments(std::string configLine);
		void			getServerBlock(ServerCluster *serverCluster, std::string configLine);
		void			parseLoop(ServerCluster *serverCluster, std::string configLine, std::string Block);
		bool			lookingForBlock(std::string configLine, std::string Block);

};

#endif