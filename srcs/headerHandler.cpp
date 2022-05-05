#include "../includes/headerHandler.hpp"
#include "../includes/response.hpp"
#include "../includes/Base64.hpp"
#include "../includes/getInfoUtils.hpp"
#include "../includes/parseUtils.hpp"
#include "../includes/manipulateUtils.hpp"
#include "../includes/headerUtils.hpp"
#include <cstdio>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>


headerHandler::headerHandler() :
	_writeToFile(false),
	_readFromFile(false),
	_writeToBrowser(false),
	_status(OK),
	_bytesWritten(0),
	_bytesRead(0),
	_maxFileSize(0),
	_contentLength(0),
	_locationIndex(-1),
	_contentType("Content-Type: text/"),
	_contentLanguage("en"),
	_authBasic("off"),
	_authType("Basic")
	{
		_statusPhrases.insert(std::pair<int, std::string>(200, "OK"));
		_statusPhrases.insert(std::pair<int, std::string>(201, "Created"));
		_statusPhrases.insert(std::pair<int, std::string>(204, "No Content"));
		_statusPhrases.insert(std::pair<int, std::string>(301, "Moved Permanently"));
		_statusPhrases.insert(std::pair<int, std::string>(307, "Temporary Redirect"));
		_statusPhrases.insert(std::pair<int, std::string>(400, "Bad Request"));
		_statusPhrases.insert(std::pair<int, std::string>(401, "UNAUTHORIZED"));
		_statusPhrases.insert(std::pair<int, std::string>(403, "Forbidden"));
		_statusPhrases.insert(std::pair<int, std::string>(404, "Not Found"));
		_statusPhrases.insert(std::pair<int, std::string>(405, "Method Not Allowed"));
		_statusPhrases.insert(std::pair<int, std::string>(413, "Request Entity Too Large"));

		_typeFunctionMap.insert(std::make_pair("Host:", &headerHandler::parseRequestedHost));
		_typeFunctionMap.insert(std::make_pair("User-Agent:", &headerHandler::parseUserAgent));
		_typeFunctionMap.insert(std::make_pair("Accept-Charset:", &headerHandler::parseAcceptCharset));
		_typeFunctionMap.insert(std::make_pair("Accept-Language:", &headerHandler::parseAcceptLanguage));
		_typeFunctionMap.insert(std::make_pair("Authorization:", &headerHandler::parseAuthorization));
		_typeFunctionMap.insert(std::make_pair("Referer:", &headerHandler::parseReferer));
		_typeFunctionMap.insert(std::make_pair("Content-Length:", &headerHandler::parseContentLength));
		_typeFunctionMap.insert(std::make_pair("Content-Type:", &headerHandler::parseContentType));
		_typeFunctionMap.insert(std::make_pair("Content-Language:", &headerHandler::parseContentLanguage));
		_typeFunctionMap.insert(std::make_pair("Content-Location:", &headerHandler::parseContentLocation));
		_typeFunctionMap.insert(std::make_pair("X-", &headerHandler::parseSpecialXHeader));
}

headerHandler::~headerHandler()
{
	resetHandler();
	_statusPhrases.clear();
	_typeFunctionMap.clear();
}

void	headerHandler::parseRequest(requestBuf requestBuffer)
{
	std::vector<std::string>	requestElements  = manipulateUtils::strToVector(requestBuffer.getHeaders());

	headerUtils::setMethod(*requestElements.begin(), _method);
	headerUtils::setUriLocation(*requestElements.begin(), _uriLocation, _body);
	headerUtils::setProtocol(*requestElements.begin(), _protocol);

	for (std::vector<std::string>::iterator it = requestElements.begin(); it != requestElements.end(); it++)
	{
		if (!it->empty())
		{
			std::string key = getInfoUtils::findFirstWord(*it);
			if (parseUtils::isHeaderKey(key) == true)
				(this->*(this->_typeFunctionMap.at(key)))(*it);
		}
	}
	parseBody(requestBuffer);

}


void	headerHandler::parseBody(requestBuf request)
{
	std::list<std::string> body = request.getBody();
	_body.reserve(request.getBodySize());
	if (_contentLength)
	{
		for(std::list<std::string>::iterator it = body.begin(); it != body.end(); it++)
			_body.append(*it);
		return ;
	}
}

void	headerHandler::parseRequestedHost(const std::string &str) {
	_requestedHost = getInfoUtils::getSingleWordFromString(str);
}

void	headerHandler::parseUserAgent(const std::string &str) {
	_userAgent = getInfoUtils::getSingleWordFromString(str);
}

void	headerHandler::parseAcceptCharset(const std::string &str) {
	_acceptCharset = getInfoUtils::getSingleWordFromString(str);
}

void	headerHandler::parseAcceptLanguage(const std::string &str) {
	_acceptLanguage = getInfoUtils::getSingleWordFromString(str);
}

void	headerHandler::parseAuthorization(const std::string &str) {
	_authorization = getInfoUtils::removeFirstWord(str);
}

void	headerHandler::parseReferer(const std::string &str) {
	_referer = getInfoUtils::getSingleWordFromString(str);
}

void	headerHandler::parseContentLength(const std::string &str) {
	_contentLength = getInfoUtils::getNumberFromString(str);
}

void	headerHandler::parseContentType(const std::string &str) {
	_contentType = getInfoUtils::getSingleWordFromString(str);
}

void	headerHandler::parseContentLanguage(const std::string &str) {
	_contentLanguage = getInfoUtils::getSingleWordFromString(str);
}

void	headerHandler::parseContentLocation(const std::string &str) {
	_contentLocation = getInfoUtils::getSingleWordFromString(str);
}

void	headerHandler::parseSpecialXHeader(const std::string &str) {
	_specialXHeader.push_back(str);
}

void headerHandler::verifyAuthorization(locationContext locationBlock, bool *authorizationStatus)
{
	if (_status >= 300 && _status <= 310)
		return;
	if (locationBlock.getAuthBasic() != "off" && !*authorizationStatus)
	{
		if (_authorization.empty())
		{
			_status = UNAUTHORIZED;
			_authBasic = locationBlock.getAuthBasic();
		}
		else
		{
			_status = FORBIDDEN;
			*authorizationStatus = false;
			if (getInfoUtils::validateUserPassword(locationBlock.getUserAndPassword(), _authorization) == true)
			{
				_status = OK;
				*authorizationStatus = true;
			}
		}
	}
}

int	headerHandler::setErrorPage(std::string &errorPage)
{
	int	fd;

	_fileLocation = errorPage.append(getInfoUtils::intToString(_status));
	_fileLocation.append(".html");
	if ((fd = open(&_fileLocation[0], O_RDONLY)) == -1)
		throw std::string("Open failed");
	return fd;
}

int headerHandler::handleRequest(std::string cgiFileTypes, std::vector<locationContext> locationBlocks, std::string errorPage, bool *authorizationStatus)
{
	struct  stat stats;
	int		fd = UNUSED;

	verifyFileLocation(locationBlocks, errorPage);
	verifyMethod();
	if (_locationIndex != -1)
		verifyAuthorization(locationBlocks[_locationIndex], authorizationStatus);
	if (_status < MOVED_PERMANENTLY)
	{
		if (_method == "POST" && cgiFileTypes.find(headerUtils::contentType(_fileLocation)) == std::string::npos)
			fd = postRequest(_maxFileSize);
		else if (_method == "DELETE")
			headerUtils::removeFile(_fileLocation, _status);
		else if (cgiFileTypes.find(headerUtils::contentType(_fileLocation)) != std::string::npos)
		    return createCgiFD("output");
		else if (stat(_fileLocation.c_str(), &stats) == -1)
			_status = NOT_FOUND;
		else if (!(stats.st_mode & S_IRUSR))
			_status = FORBIDDEN;
		else if ((fd = open(&_fileLocation[0], O_RDONLY)) == -1) {
			throw (std::string("Open failed"));
		}
	}
	if (_status >= ERROR_CODE)
		fd = setErrorPage(errorPage);
	if (fd != -1)
		if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
			throw std::string("FCNTL failed");
	return fd;
}

std::string	headerHandler::locationOfUploadedFile(locationContext locationBlock, std::string root, std::string uriLocation, std::string extension)
{
	struct stat	s;
    std::string	locationFromUri;
	std::string	directory = root;
	std::string	result = "not found";

    locationFromUri = headerUtils::getRoot(uriLocation);
    if (uriLocation.find("directory") == std::string::npos)
	    directory.append(headerUtils::getRoot(uriLocation));
    if (stat(directory.c_str(), &s) == 0 && (s.st_mode & S_IFDIR) && locationFromUri.compare(locationBlock.getLocationContext()) == 0)
	{
        result = root;
        result.append(uriLocation);
    }

    if ((s.st_mode & S_IFREG) && (locationFromUri.compare(locationBlock.getLocationContext()) == 0 && (_method == "POST" && extension == ".bla")))
		result = root;
	return result;
}

std::string	headerHandler::returnHTTPRedirect(locationContext &locationBlock)
{
	_status = locationBlock.getReturn().begin()->first;
	_fileLocation = locationBlock.getReturn().begin()->second;
	return "http redirect";
}

std::string	headerHandler::setParamsHeader(locationContext &locationBlock, const size_t &index)
{
	_maxFileSize = locationBlock.getMaxFileSize();
	_allow = locationBlock.getMethod();
	_locationBlockRoot = locationBlock.getRoot();
	_locationIndex = index;
	return locationBlock.getLocationContext();
}

std::string	headerHandler::matchLocationBlock(std::vector<locationContext> locationBlocks, std::string uriLocation)
{
	std::string	extension = headerUtils::getExtension(uriLocation);
	std::string	refererLocation = headerUtils::getRefererPart(_referer, _requestedHost);

	manipulateUtils::createPath(refererLocation);
	manipulateUtils::createPath(uriLocation);

	for (size_t index = 0; index < locationBlocks.size(); index++)
	{
		locationContext locationBlock = locationBlocks[index];
		std::string	result;

		if (_uriLocation == locationBlock.getLocationContext() && !locationBlock.getReturn().empty())
			return returnHTTPRedirect(locationBlock);

		std::string locationContext = setParamsHeader(locationBlock, index);
		result = _locationBlockRoot;

		if (!_referer.empty())
			headerUtils::setResult(result, refererLocation);

		if (locationBlock.getAlias() && locationContext.compare(headerUtils::getRoot(uriLocation)) == 0)
			result.append(headerUtils::getPathMinusRoot(uriLocation));
		else if (_method.compare("POST") == 0 && (extension != ".php" || extension == ".bla"))
			result.append("");
		else
			result.append(uriLocation);
		if (_method.compare("POST") == 0 && (extension != ".php" || extension == ".bla"))
			result = locationOfUploadedFile(locationBlock, result, uriLocation, extension);
		else
			result = headerUtils::getFile(locationBlock, result);
		if (result.compare("not found") != 0)
			return result;
	}
	_allow.clear();
	_allow.push_back("GET");
	if (_method.compare("DELETE") == 0)
		_allow.push_back("DELETE");
	return "not found";
}

std::string	headerHandler::verifyContentType()
{
	return headerUtils::contentType(_fileLocation);
}

void	headerHandler::verifyFileLocation(std::vector<locationContext> locationBlocks, std::string errorPage)
{
	std::string result = matchLocationBlock(locationBlocks, _uriLocation);

	if (result.compare("not found") == 0)
		_fileLocation = headerUtils::generateErrorPageLocation(errorPage, _uriLocation, _locationIndex);
	else if (result.compare("http redirect") == 0)
		return;
	else
		_fileLocation = result;
	manipulateUtils::createPath(_fileLocation);
}

void	headerHandler::verifyMethod(void)
{
	if (_status == MOVED_PERMANENTLY || _status == REDIRECT)
		return;
    if (_method == "POST" && headerUtils::contentType(_fileLocation) == "bla")
        return;
	else if (!_allow.empty())
		for (std::vector<std::string>::iterator it = _allow.begin(); it != _allow.end(); it++)
			if (*it == _method)
				return;
	_status = NOT_ALLOWED;
}

int headerHandler::createCgiFD(std::string type)
{
	int			cgiFD;
	std::string	name;
	const char	*filename;

	name = "serverFiles/www/temporaryFiles/cgi_out";
	if (type == "input")
		name = "serverFiles/www/temporaryFiles/cgi_in";
	filename = name.c_str();
	if ((cgiFD = open(filename, O_CREAT | O_RDWR | O_TRUNC, S_IRWXU)) == -1)
		throw std::string("Open cgi fd failed");
	if (fcntl(cgiFD, F_SETFL, O_NONBLOCK) == -1)
		throw std::string("FCNTL failed");
	return cgiFD;
}

int	headerHandler::postRequest(int maxFileSize)
{
	static int i = 0;
	std::string name = getInfoUtils::createFileName(_body);
    std::string fileName = "serverFiles/www/downloads/";
	if (name.empty())
	{
		++i;
		fileName.append("File" + getInfoUtils::intToString(i));
	}
	else
		fileName.append(name);
    int fd = UNUSED;
    _status = PAYLOAD;
    if (maxFileSize && maxFileSize < (int)_body.length())
        return fd;
    _status = OK;
    if ((fd = open(&fileName[0], O_RDWR | O_TRUNC | O_CREAT, S_IRWXU)) == -1)
        throw std::string("Open post request failed");
    return fd;
}

void	headerHandler::writeBodyToFile(int file_fd)
{
    if (_body.empty())
        return;
    if ((_bytesWritten = write(file_fd, _body.c_str(), _body.length())) == -1)
        throw std::string("Write body to file failed");
}

void	headerHandler::tryToExecute(t_cgi &data)
{
	try
	{
		char	**args = createCgiArgs();
		char 	**envp = createCgiEnvp(data);

		if (write(data.inputFD, _body.c_str(), _body.size()) != (int)_body.size() ||
			lseek(data.inputFD, 0, SEEK_SET) == -1 ||
			dup2(data.inputFD, STDIN_FILENO) == -1 ||
			dup2(data.outputFD, STDOUT_FILENO) == -1 ||
			execve(args[0], args, envp) == -1)
				throw std::string("Trying to execute failed");
	}
	catch (std::string &e)
	{
		std::cout << e << std::endl;
	}
}

void	setData(t_cgi &data, int inputFD, int outputFD, std::string serverName, int serverPort, bool authStatus, std::string authInfo)
{
	data.inputFD = inputFD;
	data.outputFD = outputFD;
	data.serverName = serverName;
	data.serverPort = serverPort;
	data.authInfo = authInfo;
	data.authSatus = authStatus;
}

void	headerHandler::executeCgi(int inputFD, int outputFD, std::string serverName, int serverPort, bool authStatus, std::string authInfo)
{
	t_cgi	data;
	pid_t	pid;
	int 	status = 0;

	setData(data, inputFD, outputFD, serverName, serverPort, authStatus, authInfo);
	if ((pid = fork()) == -1)
		throw std::string("fork() failed");
	if (pid == 0)
		tryToExecute(data);
	else
	{
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE)
			throw std::string("execute cgi child process failure");
		_bytesWritten = _body.size();
    }
}

char	**headerHandler::createCgiArgs()
{
	char	**args = new char *[3];
	char 	serverRoot[PATH_MAX];

	if (!getcwd(serverRoot, (size_t)PATH_MAX))
		throw std::string("getcwd() failed");
	if (_fileLocation.find(".php") != std::string::npos)
		args[0] = strdup("/usr/bin/php");
	else if (_fileLocation.find(".bla") != std::string::npos)
		args[0] = manipulateUtils::ft_strjoin(serverRoot, "/testFiles/cgi_tester");
	char *tmp = manipulateUtils::ft_strjoin(serverRoot, "/");
	args[1] = manipulateUtils::ft_strjoin(tmp, _fileLocation.c_str());
	args[2] = NULL;

	return args;
}

static std::string	authUser(t_cgi data)
{
	if (data.authSatus)
	{
		std::string encoded = data.authInfo.substr(7, std::string::npos);
		std::string decoded = base64_decode(encoded.c_str());
		std::string user = getInfoUtils::getUser(decoded);
		return "REMOTE_USER=" + user;
	}
	return "REMOTE_USER=";
}

void	headerHandler::specialXHeader(std::vector<std::string> &cgiEnv)
{
	for (std::vector<std::string>::iterator it = _specialXHeader.begin(); it != _specialXHeader.end(); it++)
	{
		std::string specialHeader = headerUtils::createXHeader(*it);
		cgiEnv.push_back(specialHeader);
		specialHeader.clear();
	}
}

char	**headerHandler::createCgiEnvp(t_cgi &data)
{
	char 	serverRoot[PATH_MAX];
	std::vector<std::string>	cgiEnv;

	headerUtils::serverRoot(serverRoot);

	cgiEnv.push_back(((std::string)"AUTHTYPE=").append(_authType));
	cgiEnv.push_back(((std::string)"CONTENTLENGTH=").append(getInfoUtils::intToString(_contentLength)));
	cgiEnv.push_back(((std::string)"CONTENTTYPE=").append(_contentType));
	cgiEnv.push_back((std::string)"GATEWAY_INTERFACE=CGI/1.1");
	cgiEnv.push_back(((std::string)"PATH_INFO=").append(_uriLocation));
	cgiEnv.push_back(((((std::string)"PATH_TRANSLATED=").append(serverRoot)).append("/")).append(_locationBlockRoot).append(_uriLocation));
	cgiEnv.push_back(((std::string)"QUERY_STRING=").append(_body));
	cgiEnv.push_back(((std::string)"REMOTE_ADDR=localhost"));
	cgiEnv.push_back(authUser(data));
	cgiEnv.push_back(((std::string)"REQUEST_METHOD=").append(_method));
	cgiEnv.push_back(((std::string)"SCRIPT_NAME=").append(headerUtils::getLocationWithoutRoot(_uriLocation)));
	cgiEnv.push_back(((std::string)"SERVER_NAME=").append(data.serverName));
	cgiEnv.push_back(((std::string)"SERVER_PORT=").append(getInfoUtils::intToString(data.serverPort)));
	cgiEnv.push_back(((std::string)"SERVER_PROTOCOL=").append(getProtocol()));
	cgiEnv.push_back((std::string)"SERVER_SOFTWARE=HTTP 1.1");

	specialXHeader(cgiEnv);
	return headerUtils::fromVectorToChar(cgiEnv);
}

int	headerHandler::readRequestedFile(int fd)
{
    int     ret;
    char    buff[6000000];

    if (lseek(fd, _bytesRead, SEEK_SET) == -1)
        throw std::string("lseek() failed");
    if ((ret = read(fd, buff, 6000000)) == -1)
        throw std::string("Read request file failed");
    _responseFile.append(buff, ret);
    _bytesRead += ret;
    return ret;
}

int	headerHandler::readCgiOutputFile(int fd, int body_size)
{
    int         ret;
    char        buff[6000000];

    if (_bytesRead == 0)
        _responseFile.reserve(body_size + 500);
    if (lseek(fd, _bytesRead, SEEK_SET) == -1)
		throw std::string("lseek() failed");
    if ((ret = read(fd, buff, 6000000)) == -1)
        throw std::string("Read cgi file failed");
    _responseFile.append(buff, ret);
    _bytesRead += ret;
    if (ret)
        return ret;

    if (_bytesRead)
	{
        int header_size = (int) _responseFile.find(WHITESPACE);
        _additionalCgiHeaders = _responseFile.substr(0, header_size);
        _responseFile = _responseFile.substr((header_size + 4), _responseFile.length() - (header_size + 4));
    }
    return ret;
}

void	headerHandler::createResponse(int fileFD, std::string serverName)
{
    response response;

    response.allocateSize(_responseFile);
    response.generateStatusLine(_protocol, _status, _statusPhrases);
	response.generateServerName(serverName);
	response.generateDate();
	response.generateContentLength(_responseFile);
	if (_status == UNAUTHORIZED)
		response.generateWwwAuthorization(_authBasic);
	if (_status != MOVED_PERMANENTLY && _status != REDIRECT)
	{
		response.generateContentType(headerUtils::contentType(_fileLocation));
		response.generateLastModified(fileFD);
		response.generateContentLanguage();
	}
	if (_status == NOT_ALLOWED)
		response.generateAllow(_allow);
	if (_status == CREATED || _status == MOVED_PERMANENTLY || _status == REDIRECT || ((_method == "POST" && !_body.empty() && _uriLocation.find(".bla") == std::string::npos) && _uriLocation.find(".php") == std::string::npos))
		response.generateLocation(_status, _fileLocation);
	if (!_additionalCgiHeaders.empty())
		response.appendCgiHeaders(_additionalCgiHeaders);
	response.closeHeaderSection();

    _response.append(response.getResponse());
    if (_method != "HEAD")
        _response.append(_responseFile);
    _responseSize = _response.size();
}

void    headerHandler::sendResponse(int clientFD)
{
    int ret;

    if ((ret = write(clientFD, _response.c_str() + _bytesWritten, _response.size() - _bytesWritten)) == -1)
        throw std::string("Write to browser failed");
    else
        _bytesWritten += ret;

    if (_bytesWritten == (int)_response.size())
        resetHandler();
}

void    headerHandler::resetHandler()
{
    _status = 200;
    _contentLength = 0;
    _maxFileSize = 0;
    _bytesWritten = 0;
    _responseSize = 0;
    _writeToFile = false;
    _readFromFile = false;
    _writeToBrowser = false;
    _method.clear();
    _fileLocation.clear();
    _protocol.clear();
    _response.clear();
    _responseFile.clear();
    _requestedHost.clear();
    _userAgent.clear();
    _acceptLanguage.clear();
    _authorization.clear();
    _referer.clear();
    _body.clear();
    _contentType.clear();
    _contentLanguage = "en";
    _contentLocation.clear();
    _additionalCgiHeaders.clear();
    _allow.clear();
    _specialXHeader.clear();
    _uriLocation.clear();
    _locationBlockRoot.clear();
    _acceptCharset.clear();
	_locationIndex = -1;
	_authBasic.clear();
}

int	headerHandler::getStatus() const {
	return _status;
}

int	headerHandler::getBytesWritten() const {
	return _bytesWritten;
}

int	headerHandler::getResponseSize() const {
	return _responseSize;
}

bool	headerHandler::getWriteToFile() const {
	return _writeToFile;
}

bool	headerHandler::getReadFromFile() const {
	return _readFromFile;
}

std::string	headerHandler::getMethod() const {
	return _method;
}

std::string	headerHandler::getProtocol() const {
	return _protocol;
}

std::string	headerHandler::getAuthorization() const {
	return _authorization;
}

std::string	headerHandler::getBody() const {
	return _body;
}

void	headerHandler::setBytesWritten(const int bytes) {
	_bytesWritten = bytes;
}

void	headerHandler::setBytesRead(const int bytes) {
	_bytesRead = bytes;
}

void	headerHandler::setReadFromFile(const bool status) {
	_readFromFile = status;
}

void	headerHandler::setWriteToFile(const bool status) {
	_writeToFile = status;
}

