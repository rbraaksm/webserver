#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "headerHandler.hpp"

class Client {
	public:
		friend class Webserver;
		friend class fileDescriptors;

	private:
		headerHandler	_handler;
		
		bool	_authorizationStatus;
		bool 	_active;
		int		_clientFD;
		int		_fileFD;
		int		_cgiInputFD;


	public:
		Client();
		Client(int newFD);
		~Client();

		int		getClientFD();
		int		getFileFD();
		int		getCgiInputFD();
		void	setActive(bool status);
};

#endif