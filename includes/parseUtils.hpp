#include <iostream>
#include <map>
#include <vector>
#include "server.hpp"

const std::string allowedMethod[4] =
{"GET", "POST", "DELETE", "PUT"};

const std::string serverkeys[8] =
{"port", "host", "server_name", "error_page", "time_out", "cgi_file_types", "unkown", "location"};
									
const std::string locationkeys[10] =
{"root", "allowed_method", "autoindex", "index", "alias", "max_file_size", "auth_basic", "auth_user_file", "return", "unknown"};

const std::string headerkeys[11] =
{"Host:", "User-Agent:", "Accept-Charset:", "Accept-Language:", "Authorization:", "Referer:", "Content-Length:",
"Content-Type:", "Content-Language:", "Content-Location:", "X-"};

namespace parseUtils
{
    void	getInfoFromConfigFile(std::map<int, std::string> &_lines, std::string path);
    void	checkBrackets(std::map<int, std::string> &lines);
    void	isMethodAllowed(std::vector<std::string> methods, const int line);
	void	checkIfServersAreValid(std::vector<Server> servers);
	bool	isLocationKey(std::string const &key);
	bool	isServerKey(std::string const &key);
    bool	isHeaderKey(std::string const &key);
}