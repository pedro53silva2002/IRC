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

//! THIS FUNCTION ONLY EXISTS FOR SENDTOCLIENTSINCHANNEL
void	Server::sendToClient(int id, std::string sender, std::string str)
{
	std::string reply = sender + " :" + str + "\r\n";

	for (std::vector<Client>::iterator clientIt = _clients.begin();
		clientIt != _clients.end(); ++clientIt)
		{
			if (clientIt->getId() == id) {
				send(clientIt->getSocket(), reply.c_str(), reply.size(), 0);
				return ;
			}
		}
}
void	Server::sendToClientsInChannel(int i, std::string str)
{
	int	channelId = _clients[i].getChannelId();
	if (channelId == -1)
		return ;

	for (std::vector<Client>::iterator clientIt = _clients.begin();
		clientIt != _clients.end(); ++clientIt)//
	{
		if (clientIt->getChannelId() == channelId 
			/*&& clientIt->getId() != _clients[i].getId()*/)
			{
				std::string sender = _channels[channelId].getName() + " :" + _clients[i].getNick();
				sendToClient(clientIt->getId(), sender, str);
			}
	}
}


//so weird so random
int Channel::_globalChannelId;

//so weird so random
int Client::_globalId;