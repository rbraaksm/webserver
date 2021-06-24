/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: renebraaksma <renebraaksma@student.42.f      +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/06/09 12:00:58 by timvancitte   #+#    #+#                 */
/*   Updated: 2021/06/24 16:56:01 by rbraaksm      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Webserver.hpp"
#include "ConfigParser.hpp"
#include "Error.hpp"
#include "ServerCluster.hpp"

int main (int argc, char **argv)
{
	ServerCluster	serverCluster;
	ConfigParser	configParser(argv);
	if (argc == 2)
	{
		try
		{
			configParser.openConfigFile();
			configParser.parseTheConfigFile(&serverCluster);

			// serverCluster.checkDuplicatePorts();
			// serverCluster.startup();
			// serverCluster.startListening();
		}
		catch (std::exception &e)
		{
			std::cerr << e.what() << std::endl;
			return 1;
		}
	}
	else
	{
		std::cout << "Error: Please provide a .config file\n";
		std::cout << "Example: ./webserv yourfilename.config" << std::endl;
		return 0;
	}
	return 0;
}