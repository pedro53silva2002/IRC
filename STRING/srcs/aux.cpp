#include "../includes/Server.hpp"
int Channel::_globalChannelId;
int Client::_globalId;


void	serverLog(std::string nick, std::string str)
{
	std::cout << YELLOW("Server log: ") << nick << " " << str << std::endl;
}

void	Server::setPfds()
{
	_pfds.clear();
	_pfds.push_back(_srvPfd);
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
		_pfds.push_back(it->second.getPfd());
}

void	Server::exitServer()
{
	std::cout << "exiting server" << std::endl;
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
		close(it->second.getSocket());
	close(_socket);
	throw (0);
}



void	Server::sendToClient(int i, std::string str) {
	std::string reply = str + "\r\n";
	serverLog(_clients[i].getNick(), "received string: [" + str + "]");
	send(_clients[i].getSocket(), reply.c_str(), reply.size(), 0);
}
void	Server::serverBroadcast(std::string str)
{
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++)	{
		sendToClient(it->first, str);
	}
}
void	Server::channelBroadcast(std::string chName, std::string str)
{
	int chId = getChannelId(chName);
	for (std::vector<int>::iterator it = _channels[chId].getClientsInChannel().begin(); 
		it != _channels[chId].getClientsInChannel().end(); it++) {
			sendToClient(*it, str);
		}
}
void	Server::clientBroadcast(int i, std::string chName, std::string str)
{
	int chId = getChannelId(chName);
	if (chId == -1)
	{
		std::cout << "BROKE\n";
		return ;
	}
	for (std::vector<int>::iterator it = _channels[chId].getClientsInChannel().begin(); 
		it != _channels[chId].getClientsInChannel().end(); it++) {
			if (i != *it) {
				std::cout << "will send to client " << _clients[*it].getNick() << std::endl;
				sendToClient(*it, str);
			}
	}
}


int Server::getClientId(std::string name)
{
	for (size_t i = 1; i < _clients.size(); i++)
	{
		if (name == _clients[i].getNick())
			return (i);
	}
	return (-1);
}
int Server::getChannelId(std::string name)
{
	for (size_t i = 1; i < _channels.size(); i++)
	{
		if (name == _channels[i].getName())
			return (_channels[i].getId());
	}
	return (-1);
}
std::string Server::getClientNick(int id)
{
	return (_clients[id].getNick());
}
std::string Server::getChannelName(int id)
{
	return (_channels[id].getName());
}

bool Server::isUserInChannel(int i, int chId)
{
	for (std::vector<int>::iterator it = _channels[chId].getClientsInChannel().begin(); 
		it != _channels[chId].getClientsInChannel().end(); it++) {
			if (i == *it)
				return (true);
	}
	return (false);
}

