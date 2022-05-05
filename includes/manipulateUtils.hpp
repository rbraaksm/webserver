#include <iostream>
#include <map>
#include <vector>

namespace manipulateUtils
{
	void	removeComments(std::string &line);
	void	removeSpacesBeforeAfter(std::string *line);
	void	deleteSpacesBeforeAndAfter(std::map<int, std::string> &lines);
	void	createPath(std::string &path);
	char	*ft_strjoin(char const *s1, char const *s2);
	std::vector<std::string>     strToVector(std::string request);
}