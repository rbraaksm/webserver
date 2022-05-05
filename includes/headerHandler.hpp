#ifndef HEADERHANDLER_HPP
# define HEADERHANDLER_HPP

# include <iostream>
# include <vector>
# include <map>
# include <list>
# include "locationContext.hpp"
# include "requestBuf.hpp"

enum	status_values
{
	OK = 200,
	CREATED = 201,
	NO_CONTENT = 204,
	MOVED_PERMANENTLY = 301,
	REDIRECT = 307,
	UNAUTHORIZED = 401,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	NOT_ALLOWED = 405,
	PAYLOAD = 413
};

const std::string WHITESPACE =" \n\r\t\f\v";

typedef struct s_cgi
{
	std::string	serverName;
	std::string	authInfo;
	int			inputFD;
	int			outputFD;
	int			serverPort;
	bool		authSatus;
}				t_cgi;

class headerHandler
{
	private:
		typedef void	(headerHandler::*setter)(const std::string&);
		std::map<std::string, setter>	_typeFunctionMap;
		std::map<int, std::string>		_statusPhrases;
		std::vector<std::string>		_allow;
		std::vector<std::string>		_specialXHeader;

		bool	_writeToFile;
		bool	_readFromFile;
		bool	_writeToBrowser;

		int	_status;
		int	_bytesWritten;
		int	_bytesRead;
		int	_maxFileSize;
		int	_contentLength;
		int	_locationIndex;
		int	_responseSize;

		std::string		_contentType;
		std::string		_contentLanguage;
		std::string		_contentLocation;
		std::string		_method;
		std::string		_fileLocation;
		std::string 	_uriLocation;
		std::string		_locationBlockRoot;
		std::string		_protocol;
		std::string		_requestedHost;
		std::string		_userAgent;
		std::string     _acceptCharset;
		std::string		_acceptLanguage;
		std::string		_authorization;
		std::string		_referer;
		std::string		_body;
		std::string     _authBasic;
		std::string     _authType;
		std::string     _response;
		std::string		_responseFile;
		std::string     _additionalCgiHeaders;

	public:
		headerHandler();
		~headerHandler();

		void	parseRequest(requestBuf requestBuffer);
		void	parseRequestedHost(const std::string &str);
		void	parseUserAgent(const std::string &str);
		void	parseAcceptCharset(const std::string &str);
		void	parseAcceptLanguage(const std::string &str);
		void	parseAuthorization(const std::string &str);
		void	parseReferer(const std::string &str);
		void	parseBody(requestBuf request);
		void	parseContentLength(const std::string &str);
		void	parseContentType(const std::string &str);
		void	parseContentLanguage(const std::string &str);
		void	parseContentLocation(const std::string &str);
		void	parseSpecialXHeader(const std::string &str);
		void	verifyFileLocation(std::vector<locationContext> locationBlocks, std::string errorPage);
		void	verifyMethod();
		void	verifyAuthorization(locationContext locationBlock, bool *authorizationStatus);
		void	setBytesWritten(const int bytes);
		void	setBytesRead(const int bytes);
		void	setWriteToFile(const bool status);
		void	setReadFromFile(const bool status);
		void	writeBodyToFile(int file_fd);
		void	executeCgi(int inputFD, int outputFD, std::string serverName, int serverPort, bool authStatus,
				std::string authInfo);
		void	createResponse(int fileFD, std::string serverName);
		void	sendResponse(int clientFD);
		void	resetHandler();
		void	tryToExecute(t_cgi &data);
		void	specialXHeader(std::vector<std::string> &cgiEnv);

		int	handleRequest(std::string cgiFileTypes, std::vector<locationContext> locationBlocks,
			std::string error_page, bool *authorizationStatus);
		int	postRequest(int maxFileSize);
		int	createCgiFD(std::string type);
		int	readRequestedFile(int fd);
		int	readCgiOutputFile(int fd, int bodySize);
		int	getStatus() const;
		int	getBytesWritten() const;
		int	getResponseSize() const;
		int	setErrorPage(std::string &errorPage);

		std::string	matchLocationBlock(std::vector<locationContext> locationBlocks, std::string fileLocation);
		std::string	locationOfUploadedFile(locationContext locationBlock, std::string location,
					std::string uriLocation, std::string extension);
		std::string	verifyContentType();
		std::string	getMethod() const;
		std::string	getProtocol() const;
		std::string	getAuthorization() const;
		std::string	getBody() const;
		std::string	returnHTTPRedirect(locationContext &locationBlock);
		std::string	setParamsHeader(locationContext &locationBlock, const size_t &index);

		char	**createCgiArgs();
		char	**createCgiEnvp(t_cgi &data);
		
		bool	getWriteToFile() const;
		bool	getReadFromFile() const;
};

#endif
