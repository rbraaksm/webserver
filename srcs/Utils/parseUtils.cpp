#include "../../includes/parseUtils.hpp"
#include "../../includes/fileUtils.hpp"
#include "../../includes/manipulateUtils.hpp"
#include "../../includes/error.hpp"
#include <fstream>
#include <algorithm>

namespace parseUtils
{
	void	checkIfServersAreValid(std::vector<Server> servers)
	{
		std::vector<Server>::iterator 			it = servers.begin();
		std::map<int, std::string>				ports;
		std::map<int, std::string>::iterator	pit;

		for (; it != servers.end(); ++it)
		{
			if (it->getPort() <= 0)
				throw parseError(it->getServerName(), "port");
			if (it->getTimeOut() <= 0)
				throw parseError(it->getServerName(), "time_out");
			if (it->getErrorPage().empty())
				throw parseError(it->getServerName(), "error page location");
			if ((pit = ports.find(it->getPort())) == ports.end())
				ports.insert(std::pair<int, std::string>(it->getPort(), it->getServerName()));
			else
				throw parseError("Duplicate ports in: ", pit->second, it->getServerName());
		}
	}

	void	isMethodAllowed(std::vector<std::string> methods, const int line)
	{
		std::vector<std::string>::iterator it = methods.begin();

		for (; it != methods.end(); ++it)
		{
			int i = 0;
			for (; *it != allowedMethod[i]; ++i)
				if (i == 4)
					throw parseError(line, *it);
		}
	}

	bool	isHeaderKey(std::string const &key)
	{
		for (int i = 0; i < 11; ++i)
			if (key.compare(headerkeys[i]) == 0)
					return true;
		return false;
	}

	bool	isServerKey(std::string const &key)
	{
		for (int i = 0; i < 8; ++i)
			if (key.compare(serverkeys[i]) == 0)
					return true;
		return false;
	}

	bool	isLocationKey(std::string const &key)
	{
		for (int i = 0; i < 10; ++i)
			if (key.compare(locationkeys[i]) == 0)
				return true;
		return false;
	}

	void	checkBrackets(std::map<int, std::string> &lines)
	{
		std::map<int, std::string>::iterator it = lines.begin();
		size_t open = 0;
		size_t closed = 0;

		for (; it != lines.end(); ++it)
		{
			open += std::count(it->second.begin(), it->second.end(), '{');
			closed += std::count(it->second.begin(), it->second.end(), '}');
			if (open < closed)
				throw std::invalid_argument("Error brackets aren't open");
		}
		--it;
		if (open > closed)
			throw std::invalid_argument("Error brackets aren't closed");
	}

	void	getInfoFromConfigFile(std::map<int, std::string> &_lines, std::string path)
	{
		std::fstream	fd;

		fileUtils::openFile(fd, path);
		fileUtils::getLinesFromTextFile(fd, _lines);
		parseUtils::checkBrackets(_lines);
		manipulateUtils::deleteSpacesBeforeAndAfter(_lines);
		fileUtils::closeFile(fd);
	}
}