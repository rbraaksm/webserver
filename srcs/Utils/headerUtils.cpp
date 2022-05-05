#include "../../includes/headerHandler.hpp"
#include "../../includes/headerUtils.hpp"
#include "../../includes/manipulateUtils.hpp"
#include <sys/stat.h>
#include <unistd.h>

namespace headerUtils
{
	std::string contentType(std::string &fileLocation)
	{
		for (int i = 0; i < 6; ++i)
			if (fileLocation.find(extensions[i]) != std::string::npos)
				return extensions[i];
		return "folder";
	}

	void	setMethod(std::string &str, std::string &method)
	{
		size_t	start = (str[0] == '\n') ? 1 : 0;
		size_t	end = str.find_first_of(' ', start);
		method = str.substr(start, end - start);
		str = str.substr(method.size() + 1);
	}

	void	setUriLocation(std::string &str, std::string &location, std::string &body)
	{
		if (str.find('?', 0) != std::string::npos)
		{
			location = str.substr(0, str.find('?', 0));
			body = str.substr(str.find('?', 0) + 1, (str.find(' ', 0) - location.size()));
		}
		else
			location = str.substr(0, str.find_first_of(' ', 0));
		str = str.substr(str.find_first_of(' ', 0));
	}

	void	setProtocol(std::string &str, std::string &protocol)
	{
		protocol = str.substr(str.find_first_not_of(WHITESPACE));
	}

	void	removeFile(std::string &location, int &status)
	{
		if (remove(location.c_str()))
			status = NOT_FOUND;
	}

	std::string getRefererPart(const std::string referer, const std::string requestedHost)
	{
		int			start;
		std::string	result;

		if (referer.empty())
			return "";
		start = referer.find(requestedHost);
		start += requestedHost.length();
		result = referer.substr(start, std::string::npos);
		if (result.empty())
			return "";
		return result;
	}
		
	void	setResult(std::string &result, const std::string &referer)
	{
		struct stat	s;
		result.append(referer);
		if (stat(result.c_str(), &s) == 0)
			if (s.st_mode & S_IFREG)
				result = result.substr(0, result.find_last_of('/'));
	}

	std::string	generateErrorPageLocation(std::string error_page, const std::string &uriLocation, int &locationIndex)
	{
		locationIndex = -1;
		std::string temp = uriLocation.substr(uriLocation.find_last_of('/') + 1);
		return error_page.append(temp);
	}

	std::string	getLocationWithoutRoot(const std::string &file_location)
	{
		return file_location.substr(file_location.find_last_of("/") + 1);
	}

	std::string getRoot(std::string path)
	{
		if (path.find_first_of('/', 1) != std::string::npos)
			return path.substr(0, path.find_first_of('/', 1));
		return path;
	}

	std::string getPathMinusRoot(std::string path)
	{
		if (path.find_first_of('/', 1) != std::string::npos)
			return path.substr(path.find_first_of('/', 1), std::string::npos);
		return "";
	}

	std::string	filePath(struct stat s, locationContext location_block, std::string location)
	{
		std::string result = location;
		std::string tmp = location;
		
		if (s.st_mode & S_IFDIR)
		{
			if (location_block.getIndex().empty())
				return "not found";
			tmp.append(location_block.getIndex());
			if (stat(tmp.c_str(), &s) == -1)
			{
				if (location_block.getAutoIndex())
					result.append("/index.php");
				else
					return "not found";
			}
			else
				result.append(location_block.getIndex());
			return result;
		}
		else if (s.st_mode & S_IFREG)
			return result;
		return location;
	}

	std::string	getFile(locationContext location_block, std::string location)
	{
		struct stat	s;

		manipulateUtils::createPath(location);

		if (stat(location.c_str(), &s) == 0)
			return filePath(s, location_block, location);
		else
			return "not found";
	}

	std::string	getExtension(std::string str)
	{
		if (str.find_last_of('.') == std::string::npos)
			return str;
		return str.substr(str.find_last_of('.'), std::string::npos);
	}

    std::string createXHeader(std::string &str)
    {
        std::string special_header = "HTTP_";
        for (size_t i = 0; i < str.size(); i++)
        {
            char c = *(str.c_str() + i);
            if (c == '-')
                special_header.push_back('_');
            else if (c == ':')
                special_header.push_back('=');
            else if (c != ' ')
                special_header.push_back(toupper(c));
        }

        return special_header;
    }

    void	serverRoot(char *str)
    {
        if (!getcwd(str, (size_t)PATH_MAX))
            throw (std::string("getcwd() failed"));
    }

    char	*strdup(const char *str)
    {
        int		index;
        char	*result;

        result = (char *)malloc(strlen(str) + 1);
        if (!result)
            throw (std::string("Malloc failed"));
        index = 0;
        while (str[index] != '\0')
        {
            result[index] = str[index];
            index++;
        }
        result[index] = '\0';
        return result;
    }
    
    char    **fromVectorToChar(std::vector<std::string> env)
    {
        char 	**envp = new char *[env.size() + 1];
        int		i = 0;

        for (std::vector<std::string>::iterator it = env.begin(); it != env.end(); it++) {
            envp[i] = strdup((*it).c_str());
            i++;
        }
        envp[env.size()] = NULL;
        return envp;
    }
}