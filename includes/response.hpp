#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <map>
# include <vector>

class response {
	private:
		std::string	_response;

	public:
		response();
		~response();

		void	allocateSize(std::string requestedFile);
		void	generateStatusLine(std::string protocol, int status, std::map<int, std::string> statusPhrases);
		void	generateContentLength(std::string requestedFile);
		void	generateContentType(std::string type);
		void	generateContentLanguage();
		void	generateLastModified(int file_fd);
		void	generateDate();
		void	generateServerName(std::string serverName);
		void	generateAllow(std::vector<std::string> allowedMethods);
		void	generateLocation(int status, std::string fileLocation);
		void	generateConnectionClose();
		void	generateWwwAuthorization(std::string authBasic);
		void	appendCgiHeaders(std::string cgiHeaders);
		void	closeHeaderSection();

		std::string	getResponse();
};

#endif
