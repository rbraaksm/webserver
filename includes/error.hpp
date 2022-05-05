#ifndef ERROR_HPP
# define ERROR_HPP

# include <iostream>

class parseError : public std::exception
{
		public:
			parseError(std::string line)
				: _error("Error: opening file with path: " + line){}

			parseError(std::string line, std::string line2)
				: _error("Error: servername: " + line + " : " + line2 + " is invalid/missing"){}

			parseError(std::string line, std::string line2, std::string line3)
				: _error("Error: " + line + line2 + " and " + line3){}

			parseError(std::string line, size_t lineCount)
				: _error("Error: [line " + std::to_string(lineCount) + "] " + line){}

			parseError(std::string line, std::string line2, size_t lineCount)
				: _error("Error: [line " + std::to_string(lineCount) + "] " + line + ": '" + line2 + "' not found") {}

			parseError(size_t lineCount, std::string line)
					: _error("Error: [line " + std::to_string(lineCount) + "] Method: '" + line + "' not allowed") {}

			const char *what() const throw() {return _error.c_str();}

			virtual ~parseError() throw() {}

		private:
			std::string _error;
};

#endif
