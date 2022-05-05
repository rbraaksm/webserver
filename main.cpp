#include "includes/webserver.hpp"

void	signal_handler(int signal)
{
	if (signal == SIGINT)
	{
		std::cout << RED << " Stopping server..." << RESET << std::endl;
		exit(0);
	}
}

static std::string	checkInput(int ac, char **av)
{
	if (ac > 2)
	{
		std::cout << "How to run: execute " << GREEN
		<< "./webserv " << RESET << "or type" << GREEN
		<< " './webserv [configFile]'" << RESET << std::endl;
		return "";
	}
	std::string ret = "confFiles/valid/default.conf";
	if (av[1])
		ret = av[1];
	return ret;
}

int main(int ac, char **av)
{
	Webserver	webserver;
	
	std::string	path = checkInput(ac, av);
	if (path.empty())
		return 0;
	try {
		signal(SIGINT, signal_handler);
		webserver.parseConfiguration(path);
		webserver.establishConnection();
		webserver.initializeForRun();
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}
