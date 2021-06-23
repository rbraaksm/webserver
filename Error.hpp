/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Error.hpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: timvancitters <timvancitters@student.co      +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/06/09 12:13:43 by timvancitte   #+#    #+#                 */
/*   Updated: 2021/06/16 12:38:06 by timvancitte   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_HPP
# define ERROR_HPP

#include "Webserver.hpp"

class parseError : public std::exception {
		public:
			parseError(std::string line, size_t lineCount) 
				: _error("Error: " + line + ": syntax error line " + std::to_string(lineCount)) {
			}
			parseError(std::string line, const char *message) 
				: _error("Error: line:" + line + ": " + message) {
			}
			const char *what() const throw() {
				return _error.c_str();
			}
			virtual ~parseError() throw() {}
		private:
			std::string _error;
};

class openFileError : public std::exception
{
		public:
			openFileError(const char *message, const char *configPath) 
				: _error(message + std::string(configPath)) {
			}
			openFileError(const char *message, std::string configPath) 
				: _error(message + configPath) {
			}
			const char *what() const throw() {
				return _error.c_str();
			}
			virtual ~openFileError() throw() {}
		private:
			std::string _error;
};

class startupError : public std::exception 
{
		public:
			startupError(std::string message, std::string host) 
				: _error("Error: startup failed: " + message + host) {
			}
			const char *what() const throw() {
				return _error.c_str();
			}
			virtual ~startupError() throw() {}
		private:
			std::string _error;
};

class clusterError : public std::exception 
{
		public:
			clusterError(const char *message, const char *configPath) 
				: _error(message + std::string(configPath)) {
			}
			const char *what() const throw() {
				return _error.c_str();
			}
			virtual ~clusterError() throw() {}
		private:
			std::string _error;
};

#endif