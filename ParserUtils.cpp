/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ParserUtils.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: renebraaksma <renebraaksma@student.42.f      +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/06/09 14:59:49 by timvancitte   #+#    #+#                 */
/*   Updated: 2021/06/22 11:29:05 by rbraaksm      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ParserUtils.hpp"
#include "ConfigParser.hpp"

namespace Utils
{

bool	skipEmptyLine(std::string &line) {
	size_t start = line.find_first_not_of(WHITESPACE, 0);
	return (start == std::string::npos) ? true : false;

}

std::string findFirstWord(std::string &line) {
	size_t		start;
	size_t		end;

	start = line.find_first_not_of(WHITESPACE, 0);
	end = line.find_first_of(WHITESPACE, start);
	return line.substr(start, end);

}

std::string removeLeadingSpaces(const std::string &line) {
	size_t start;

	start = line.find_first_not_of(WHITESPACE, 0);
	if (start == std::string::npos)
		return "";
	else
		return line.substr(start);
}

std::string removeTrailingSpaces(const std::string &line) {
	size_t end;

	end = line.find_last_not_of(WHITESPACE);
	if (end == std::string::npos)
		return "";
	else
		return line.substr(0, end + 1);
}

std::string removeLeadingAndTrailingSpaces(const std::string &line) {
	return removeTrailingSpaces(removeLeadingSpaces(line));
}

std::string splitUserFromString(const std::string &line, const char* delimiter) {
	std::string result;

	result = line.substr(0, (line.find_first_of(delimiter)));
	return result;
}

std::string splitPasswordFromString(const std::string &line, const char* delimiter, const char* endOfLine) {
	std::string result;
	result = line.substr(line.find_first_of(delimiter) + 1, line.find_first_of(endOfLine));
	return result;
}

bool		checkIfStringIsEmpty(const std::string &string) {
	return string.empty();
}

std::string removeSpacesFromString(std::string string) {
	string.erase(std::remove_if(string.begin(), string.end(), ::isspace), string.end());
	return string;
}

bool		checkIfUserAndPasswordInputIsValid(const std::string &line) {
	std::regex regex("^[a-zA-Z0-9._]*:{1}[a-zA-Z0-9._]*$");
	return std::regex_match(line, regex);
}

void		getKeyValue(const std::string &line, std::string &user, std::string &password, const char* delimiter, const char* endOfLine)
{
	if (checkIfUserAndPasswordInputIsValid(line) == false)
		throw parseError("regex check returns false", "please check your username and password input");
	user = splitUserFromString(line, delimiter);
	password = splitPasswordFromString(line, delimiter, endOfLine);
	if (checkIfStringIsEmpty(user) == true) {
		user = "admin";
		std::cout << "No user was added. Default username is set: [" << user << "]" << std::endl;
	}
	if (checkIfStringIsEmpty(password) == true) {
		std::cout << "No password was set for user: ["<< user << "]" << " default password is set." << std::endl;
		password = "admin";
	}
}

std::string		checkLocationPath(std::string &startLine, int lineCount)
{
	startLine.erase(0, startLine.find_first_of(WHITESPACE, 0));
	startLine = removeLeadingAndTrailingSpaces(startLine);
	if (startLine[0] == '*' && startLine[1] == '.')
		return startLine = setLocationPath(startLine, '*', lineCount);
	else if (startLine[0] == '/')
		return startLine = setLocationPath(startLine, '/', lineCount);
	else
		throw parseError("missing location path ", lineCount);
	return NULL;
}

std::string		setLocationPath(std::string &startLine, const char beginOfPathCharacter, int lineCount)
{
	size_t bracketPosition = startLine.find_first_of("{", 0);
	size_t forwardslashPosition = startLine.find_first_of(beginOfPathCharacter, 0);
	size_t endoflocationPath = startLine.find_first_of(WHITESPACE, forwardslashPosition);
	size_t checkIfOnlyBlanksBetweenPathAndBracket = startLine.find_first_not_of(WHITESPACE, endoflocationPath);

	if (bracketPosition == std::string::npos || forwardslashPosition == std::string::npos)
		throw parseError("missing block ", lineCount);
	else if (endoflocationPath == std::string::npos) // no space between path and bracket
		startLine = startLine.substr(forwardslashPosition, bracketPosition - forwardslashPosition);
	else if (checkIfOnlyBlanksBetweenPathAndBracket != bracketPosition)
		throw parseError("invalid path, character between path and '{' ", lineCount);
	else
	{
		size_t lengthOfLocationPath = endoflocationPath - forwardslashPosition;
		startLine = startLine.substr(forwardslashPosition, lengthOfLocationPath);
	}
	return startLine;
}

} // End of UTILS Namespace
