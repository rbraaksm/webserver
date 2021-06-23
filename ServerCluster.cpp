/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerCluster.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: renebraaksma <renebraaksma@student.42.f      +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/06/09 11:57:45 by timvancitte   #+#    #+#                 */
/*   Updated: 2021/06/22 10:56:05 by rbraaksm      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ServerCluster.hpp"
// #include "Connection.hpp"

// Connection	*g_recentConnection;

ServerCluster::ServerCluster() : _numberOfServers(0), _highestFD(0) {
	FD_ZERO(&this->readFds);
	FD_ZERO(&this->writeFds);
	return;
}

ServerCluster::ServerCluster(ServerCluster const &src) {
	if (this != &src)
		*this = src;
	return;
}

ServerCluster::~ServerCluster() {

	// std::vector<Server*>::iterator it = this->_allServers.begin();
	// for (; this->_allServers.empty() && it != this->_allServers.end(); ++it) {
	// 	delete(*it);
	// }
	return;
}

ServerCluster&		ServerCluster::operator=(ServerCluster const &obj) {
	if (this != &obj) {
		this->_numberOfServers = obj._numberOfServers;
		this->_allServers = obj._allServers;
		this->_doublePorts = obj._doublePorts;
		this->_highestFD = obj._highestFD;
		this->readFds = obj.readFds;
		this->writeFds = obj.writeFds;
	}
	return *this;
}

void			ServerCluster::addServer(Server *newServer) {
	this->_allServers.push_back(newServer);
}

bool			ServerCluster::clusterIsEmpty() const {
	return this->_allServers.empty();
}

// //TODO checken wat dit doet en of het werkt met meerdere Severs die dezelfde port grebruiken
// void			ServerCluster::checkDuplicatePorts() {
// 	std::vector<Server*>::const_iterator it1 = this->_allServers.begin();
// 	std::vector<Server*>::const_iterator it2;

// 	if (this->_allServers.size() < 2)
// 		return;
// 	for (; it1 != this->_allServers.end(); ++it1) {
// 		it2 = it1;
// 		it2++;
// 		while (it2 != this->_allServers.end()) {
// 			if ((*it1)->getPortNumber() == (*it2)->getPortNumber()) {
// 				(*it1)->setAlternativeServers(*it2);
// 			}
// 			++it2;
// 		}
// 	}
// }

// void	ServerCluster::startup() {
// 	std::vector<Server*>::iterator it = this->_allServers.begin();
// 	for (; !this->_allServers.empty() && it != this->_allServers.end(); ++it) {
// 		(*it)->startListening();
// 		FD_SET((*it)->getSocketFD(), &this->readFds);
// 		this->_highestFD = std::max(this->_highestFD, (*it)->getSocketFD());
// 		this->_numberOfServers += 1;
// 	}
// }

// // void	ServerCluster::startListening() {
// // 	while (true)
// // 	{
// // 		fd_set	readSet;
// // 		fd_set	writeSet;
// // 		int		ret;
// // 		long	maxFD= this->_highestFD;
// // 		std::vector<Server*>::iterator it = this->_allServers.begin();
// // 		g_recentConnection = NULL;
// // 		signal(SIGPIPE, Utils::signalHandler); // if we're trying to read or write to a socket that's no longer valid
// // 		FD_ZERO(&writeSet);
// // 		FD_ZERO(&readSet);
// // 		readSet = this->readFds;
// // 		while (it != this->_allServers.end()) {
// // 			for (int i = 0; i < NR_OF_CONNECTIONS; ++i) {
// // 				unsigned long a = Utils::getTime();
// // 				unsigned long b = (*it)->connections[i].getTimeLastRead();
// // 				if (CONNECTION_TIMEOUT > 0 && a - b > CONNECTION_TIMEOUT) {
// // 					if (!(*it)->connections[i].getBuffer().empty()) {
// // 						g_recentConnection = &((*it)->connections[i]);
// // 						(*it)->createRes
// // 					}
// // 				}
// // 			}
// // 		}
// // 	}
// // }