#include <vector>
#include <map>

namespace getInfoUtils
{
	std::vector<std::string>	getWordsFromString(const std::string &str);
	std::string	getSingleWordFromString(const std::string& str);
	std::string	findFirstWord(const std::string &line);
	std::string	createParameter(std::string configLine);
	std::string	removeFirstWord(const std::string& str);
	std::string	getUser(std::string &str);
	std::string	intToString(int nb);
	std::string	createFileName(const std::string body);
	void    setLocationContext(const std::string &str, std::string &ret);
	void	readBrowserRequest(std::string &request, int fd);
	bool	validateUserPassword(std::map<std::string, std::string> userAndPasswords, std::string authorization);
	int		getNumberFromString(const std::string& str);
}