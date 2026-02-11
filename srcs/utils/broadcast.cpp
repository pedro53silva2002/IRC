#include "../../includes/Server.hpp"

void	Server::sendToClient(int i, std::string str) {
	std::string reply = str + "\r\n";
	serverLog(_clients[i].getNick(), "received string: [" + str + "]");
	send(_clients[i].getSocket(), reply.c_str(), reply.size(), 0);
}

void	Server::serverBroadcast(std::string str)
{
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
		if (_clients[it->first].isRegistered())
			sendToClient(it->first, str);
}

void	Server::channelBroadcast(int chId, std::string str)
{
	for (std::vector<int>::iterator it = _channels[chId].getClientsInChannel().begin(); 
		it != _channels[chId].getClientsInChannel().end(); it++) {
			sendToClient(*it, str);
		}
}

void	Server::clientBroadcast(int i, int chId, std::string str)
{
	for (std::vector<int>::iterator it = _channels[chId].getClientsInChannel().begin(); 
		it != _channels[chId].getClientsInChannel().end(); it++) {
			if (i != *it)
				sendToClient(*it, str);
	}
}

