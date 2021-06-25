/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ParserUtils.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: renebraaksma <renebraaksma@student.42.f      +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/06/09 14:59:49 by timvancitte   #+#    #+#                 */
/*   Updated: 2021/06/25 09:58:22 by rbraaksm      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ParserUtils.hpp"
#include "ConfigParser.hpp"

namespace Utils
{

bool	skipEmptyLine(std::string *line) {
	size_t start = line->find_first_not_of(WHITESPACE, 0);
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

void		removeSpacesBeforeAfter(std::string *line)
{
	size_t n = line->find_last_not_of(WHITESPACE);
	size_t x = line->find_first_not_of(WHITESPACE);
	if (n != std::string::npos)
		line->assign(line->substr(0, n + 1));
	if (x != std::string::npos)
		line->assign(line->substr(x));
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

std::string		setLocationPath(std::string &startLine, size_t start, size_t end)
{
	return startLine.substr(start, end);
}

std::string		lookForWhitespaces(std::string &startLine, size_t pos_char, size_t pos_end, size_t pos_bracket, int lineCount)
{
	size_t only_whitespaces = startLine.find_first_not_of(WHITESPACE, pos_end);
	if (only_whitespaces != pos_bracket)
		throw parseError("invalid path, character between path and '{' ", lineCount);
	return setLocationPath(startLine, pos_char, (pos_end - pos_char));
}

std::string		lookForEnd(std::string &startLine, size_t pos_bracket, int lineCount)
{
	size_t pos_char = startLine.find_first_of(startLine[0], 0);
	size_t pos_end = startLine.find_first_of(WHITESPACE, pos_char);
	if (pos_end == std::string::npos)
		return setLocationPath(startLine, pos_char, (pos_bracket - pos_char));
	return lookForWhitespaces(startLine, pos_char, pos_end, pos_bracket, lineCount);

}

std::string		lookForBracket(std::string &startLine, int lineCount)
{
	size_t pos_bracket = startLine.find_first_of("{", 0);
	if (pos_bracket == std::string::npos)
		throw parseError("missing bracket ", lineCount);
	return lookForEnd(startLine, pos_bracket, lineCount);
}

std::string		checkLocationPath(std::string &startLine, int lineCount)
{
	startLine.erase(0, startLine.find_first_of(WHITESPACE, 0));
	removeSpacesBeforeAfter((&startLine));
	if ((startLine[0] == '*' && startLine[1] == '.') || startLine[0] == '/')
		return lookForBracket(startLine, lineCount);
	throw parseError("missing location path ", lineCount);
}


} // End of UTILS Namespace
