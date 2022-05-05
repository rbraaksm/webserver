#include "../../includes/fileUtils.hpp"
#include "../../includes/manipulateUtils.hpp"
#include "../../includes/error.hpp"

const std::string WHITESPACE =" \n\r\t\f\v";

namespace fileUtils
{
	bool	skipEmptyLine(std::string &line)
	{
		size_t start = line.find_first_not_of(WHITESPACE, 0);
		return (start == std::string::npos) ? true : false;
	}

	void	getLinesFromTextFile(std::fstream &fd, std::map<int, std::string> &_lines)
	{
		std::string income;
		int i = 1;
		while (std::getline(fd, income))
		{
			manipulateUtils::removeComments(income);
			if (skipEmptyLine(income) == false)
				_lines.insert(std::pair<int, std::string>(i, income));
			++i;
		}
	}

	void	openFile(std::fstream &fd, std::string path)
	{
		fd.open(path);
		if (!fd)
			throw parseError(path);
	}

	void	closeFile(std::fstream &fd)
	{
		fd.close();
	}
}