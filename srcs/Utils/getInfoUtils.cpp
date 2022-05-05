#include "../../includes/getInfoUtils.hpp"
#include "../../includes/manipulateUtils.hpp"
#include "../../includes/Base64.hpp"
#include "../../includes/error.hpp"
#include <vector>
#include <sstream>
#include <unistd.h>

const std::string WHITESPACE =" \n\r\t\f\v";

namespace getInfoUtils
{
	std::string	createFileName(const std::string body)
	{
		size_t pos = body.find("filename");
		if (pos == std::string::npos)
			return "";
		pos += 10;
		return body.substr(pos, (body.find_first_of('"', pos) - pos));
	}
	
	void readBrowserRequest(std::string &request, int fd) {
		char    buff[6000000];
		int     ret = 1;

		request.reserve(1);
		ret = read(fd, buff, 6000000);
		if (ret == -1)
		{
			close(fd);
			throw std::string("Read browser request failed");
		}
		buff[ret] = '\0';
		request.append(buff, ret);
	}

	std::string	intToString(int nb)
	{
		std::stringstream tmp;
		tmp << nb;
		return tmp.str();	
	}

	int	getNumberFromString(const std::string& str)
	{
		std::istringstream			ss(str.substr(findFirstWord(str).size()));
		std::string 				word;

		ss >> word;
		return atoi(word.c_str());
	}

	std::vector<std::string>	getWordsFromString(const std::string &str)
	{
		std::vector<std::string>	vector;
		std::istringstream			ss(str.substr(findFirstWord(str).size()));
		std::string 				word;

		while (ss >> word)
			vector.push_back(word);
		return vector;
	}

	std::string	getSingleWordFromString(const std::string& str)
	{
		std::istringstream			ss(str.substr(findFirstWord(str).size()));
		std::string 				word;

		ss >> word;
		return word;
	}

	std::string findFirstWord(const std::string &line)
	{
		size_t		start;
		size_t		end;

		start = line.find_first_not_of(WHITESPACE, 0);
		end = line.find_first_of(WHITESPACE, start);
		return line.substr(start, end);
	}

	std::string	createParameter(std::string configLine)
	{
		std::string ret;
		configLine.resize(configLine.size());
		size_t pos = configLine.find_first_of(WHITESPACE, 0);
		if (pos == std::string::npos)
			return ret;
		ret = configLine.substr(pos);
		manipulateUtils::removeSpacesBeforeAfter(&ret);
		return ret;
	}

	std::string removeFirstWord(const std::string& str)
	{
		return str.substr(findFirstWord(str).size());
	}

	std::string getUser(std::string &str)
	{
		static int	i = 1;
		size_t		start;
		size_t		end;

		start = str.find_first_not_of(WHITESPACE, 0);
		end = str.find_first_of(':', start);
		if (end == std::string::npos)
			throw parseError("there is no password in " + str, i);
		++i;
		std::string ret = str.substr(start, end);
		str = str.substr(ret.size() + 1);
		return ret;
	}

	bool	validateUserPassword(std::map<std::string, std::string> userAndPasswords, std::string authorization)
	{
		if (authorization.find("Basic") == std::string::npos)
			return false;
		std::string encoded = authorization.substr(7, std::string::npos);
		std::string decoded = base64_decode(encoded.c_str());
		if (decoded.find(":") == std::string::npos)
			return false;
		std::string user = getInfoUtils::getUser(decoded);
		std::map<std::string, std::string>::iterator it = userAndPasswords.find(user);
		if (it != userAndPasswords.end())
			if (base64_decode(it->second) == decoded)
				return true;
		return false;
	}

	void		setLocationContext(const std::string &str, std::string &ret)
	{
		size_t start = str.find_first_of('/');
		size_t end = str.find_first_of(' ', start);

		ret = str.substr(start, end - start);
	}
}