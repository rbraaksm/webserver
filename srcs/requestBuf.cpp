#include "../includes/requestBuf.hpp"

requestBuf::requestBuf() :_headers(), _body(), _bodySize(0) {
}

requestBuf::~requestBuf() {
	_headers.clear();
	_body.clear();
}

void	requestBuf::addHeaderData(const std::string &requestData) {
	size_t pos;

	_headers.append(requestData);
	if ((pos = _headers.find("\r\n\r\n")) != std::string::npos) {
		if ((pos+4) == _headers.size())
			return;
		addBodyData(_headers.substr(pos+4));
		_headers = _headers.substr(0, pos+4);
	}
}

void	requestBuf::addBodyData(const std::string &requestData) {
	_body.push_back(requestData);
	_bodySize += requestData.length();
}

int		requestBuf::headersReceived() {
	if (_headers.find("\r\n\r\n") != std::string::npos)
		return 1;
	return 0;
}

int		requestBuf::validateRequest() {
	int endOfLine = 0;
	int zero = 0;
	int pos;

	if ((pos = (int) _headers.find("Content-Length:")) != -1) {
		size_t contentLength = atoi(_headers.c_str() + (pos + 16));
		if (contentLength == _bodySize)
			return VALID;
	}
	else if (_headers.find("chunked") != std::string::npos) {
		for(std::list<std::string>::reverse_iterator it = _body.rbegin(); it != _body.rend(); it++) {
			std::string str = *it;
			for (int i = (int)str.length() - 1; i >= 0; i--) {
				if (str[i] == '\r' || str[i] == '\n')
					endOfLine++;
				else if (str[i] == '0')
					zero++;
				else
					return INVALID;
				if (zero == 1 && endOfLine == 4)
					return VALID;
			}
		}
	}
	else if (_headers.find("Content-Length:") == std::string::npos && _headers.find("chunked") == std::string::npos)
		return VALID;
	return INVALID;
}

std::string	requestBuf::getHeaders() {
	return _headers;
}

std::list<std::string>	requestBuf::getBody() {
	return _body;
}

int	requestBuf::getBodySize() {
	return _bodySize;
}

void	requestBuf::setBodySize(int size) {
	_bodySize = size;
}
