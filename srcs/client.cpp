#include "../includes/client.hpp"

Client::Client() : _authorizationStatus(false), _active(false), _clientFD(-1), _fileFD(-1), _cgiInputFD(-1) {
}

Client::Client(int newFD) : _authorizationStatus(false), _active(false), _clientFD(newFD), _fileFD(-1), _cgiInputFD(-1){
}

Client::~Client() {
}

int	Client::getClientFD() {
	return _clientFD;
}

int	Client::getFileFD() {
	return _fileFD;
}

int	Client::getCgiInputFD() {
	return _cgiInputFD;
}

void	Client::setActive(bool status) {
	_active = status;
}
