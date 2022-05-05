#include "../includes/response.hpp"
#include "../includes/getInfoUtils.hpp"
#include <sys/stat.h>

response::response() {
}

response::~response() {
	_response.clear();
}

void	response::allocateSize(std::string requestedFile) {
	int size = 500 + requestedFile.size();
	_response.reserve(size);
}

void	response::generateStatusLine(std::string protocol, int status, std::map<int, std::string> statusPhrases) {
	std::string statusLine = protocol;
	statusLine.append(" ");
	statusLine.append(getInfoUtils::intToString(status));
	statusLine.append(" ");
	statusLine.append(statusPhrases[status]);
	statusLine.append("\r\n");
	_response.append(statusLine);
}

void	response::generateContentLength(std::string requestedFile) {
	std::string	contentLength = "Content-Length: ";
	contentLength.append(getInfoUtils::intToString(requestedFile.size()));
	contentLength.append("\r\n");
	_response.append(contentLength);
}

void	response::generateContentType(std::string type) {
	std::string	contentTypeHeader = "Content-Type: ";
	std::string contentType = type;

	if (contentType.compare("php") == 0 || contentType.compare("bla") == 0){
		contentType = "html";
	}
	if (contentType.compare("html") == 0 || contentType.compare("css") == 0)
		contentTypeHeader.append("text/");
	else if (contentType.compare("png") == 0)
		contentTypeHeader.append("image/");
	contentTypeHeader.append(contentType);

	contentTypeHeader.append("\r\n");

	_response.append(contentTypeHeader);
}

void	response::generateContentLanguage() {
	_response.append("Content-Language: en-US\r\n");
}

void	response::generateLastModified(int fileFD) {
	std::string	lastModified = "Last-Modified: ";
	struct stat	stat;
	struct tm	*info;
	char		timestamp[36];

	fstat(fileFD, &stat);
	info = localtime(&stat.st_mtime);
	strftime(timestamp, 36, "%a, %d %h %Y %H:%M:%S GMT", info);
	lastModified.append(timestamp);
	lastModified.append("\r\n");

	_response.append(lastModified);
}

void	response::generateDate() {
	std::string	date = "Date: ";
	time_t		timer;
	struct tm	*info;
	char		timestamp[36];

	timer = time(NULL);
	info = localtime(&timer);
	strftime(timestamp, 36, "%a, %d %h %Y %H:%M:%S GMT", info);
	date.append(timestamp);
	date.append("\r\n");
	_response.append(date);
}

void	response::generateServerName(std::string serverName) {
	std::string serverHeader = "Server: ";
	serverHeader.append(serverName);
	serverHeader.append("\r\n");
	_response.append(serverHeader);
}

void	response::generateAllow(std::vector<std::string> allowedMethods) {
	std::string allowHeader = "Allow: ";

	for (std::vector<std::string>::iterator it = allowedMethods.begin(); it != allowedMethods.end(); it++){
		allowHeader.append(*it);
		if (it + 1 != allowedMethods.end())
			allowHeader.append(", ");
	}
	allowHeader.append("\r\n");
	_response.append(allowHeader);
}

void	response::generateLocation(int status, std::string fileLocation) {
	std::string locationHeader = "Location: ";

	if (status == 201) {
		locationHeader.append("serverFiles/www/downloads/");
		std::string file = fileLocation.substr(fileLocation.find_last_of('/') + 1);
		locationHeader.append(file);
	}
	else if (status == 301 || status == 307)
		locationHeader.append(fileLocation);
	else
		locationHeader.append("serverFiles/www/downloads/POST_file");
	locationHeader.append("\r\n");
	_response.append(locationHeader);
}

void	response::generateConnectionClose() {
	_response.append("Connection: close\r\n");
}

void	response::generateWwwAuthorization(std::string authBasic) {
	_response.append((std::string)"WWW-Authenticate: Basic realm=").append(authBasic);
	_response.append("\r\n");
}

void	response::appendCgiHeaders(std::string cgiHeaders) {
	_response.append(cgiHeaders);
	_response.append("\r\n");
}

void	response::closeHeaderSection() {
	_response.append("\r\n");
}

std::string response::getResponse() {
	return _response;
}
