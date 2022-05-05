#include <iostream>
#include <map>
#include <fstream>

namespace fileUtils
{
	void	getLinesFromTextFile(std::fstream &fd, std::map<int, std::string> &_lines);
	void	openFile(std::fstream &fd, std::string path);
	void	closeFile(std::fstream &fd);
	bool	skipEmptyLine(std::string &line);
}