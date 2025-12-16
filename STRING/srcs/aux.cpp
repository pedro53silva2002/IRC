#include "../includes/Server.hpp"

void	serverLog(std::string nick, std::string str)
{
	std::cout << YELLOW("Server log: ") << nick << " " << str << std::endl;
}

//this is just testing
void	Server::exitServer()
{
	std::cout << "exiting server" << std::endl;
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
		close(it->getSocket());
	close(_socket);
	throw (0);
}



//*SENDERS
void	Server::sendToClient(int i, std::string str) {
	std::string reply = str + "\r\n";//todo maybe its just \n
	serverLog(_clients[i].getNick(), "received string: [" + str + "]");
	send(_clients[i].getSocket(), reply.c_str(), reply.size(), 0);
}

//todo getPrefix() before this, but it needs to be sent as parameter
void	Server::sendToClientsInChannel(int i, std::string str)
{
	for (int j = 0; j < _clients.size(); j++) {
		if (_clients[i].getChannelId() == _clients[j].getChannelId() && i != j)
			sendToClient(j, str);
	}
}


//so weird so random
int Channel::_globalChannelId;

//so weird so random
int Client::_globalId;