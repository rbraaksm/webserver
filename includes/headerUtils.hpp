#include <iostream>
#include "locationContext.hpp"

const std::string extensions[6] =
{"html", "php", "css", "ico", "png", "bla"};

namespace headerUtils
{
	void	setMethod(std::string &str, std::string &method);
	void	setUriLocation(std::string &str, std::string &location, std::string &body);
	void	setProtocol(std::string &str, std::string &protocol);
	void	removeFile(std::string &location, int &status);
	void	setResult(std::string &result, const std::string &referer);
	void	serverRoot(char *str);

	std::string contentType(std::string &fileLocation);
	std::string getRefererPart(const std::string referer, const std::string requestedHost);
	std::string	generateErrorPageLocation(std::string error_page, const std::string &uriLocation, int &locationIndex);
	std::string	getLocationWithoutRoot(const std::string &file_location);
	std::string getRoot(std::string path);
	std::string getPathMinusRoot(std::string path);
	std::string getFile(locationContext location_block, std::string location);
	std::string getExtension(std::string str);
	std::string createXHeader(std::string &str);

	char	**fromVectorToChar(std::vector<std::string> env);
}