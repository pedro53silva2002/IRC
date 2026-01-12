#include "../includes/Server.hpp"
int Channel::_globalChannelId;//learn why this needs to happen
int Client::_globalId;


void	serverLog(std::string nick, std::string str)
{
	std::cout << YELLOW("Server log: ") << nick << " " << str << std::endl;
}

//*Set pollfds for loop
void	Server::setPfds()
{
	_pfds.clear();
	_pfds.push_back(_srvPfd);
	for (std::vector<Client>::iterator it = _clients.begin() + 1; it != _clients.end(); it++)
		_pfds.push_back(it->getPfd());
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
	std::string reply = str + "\r\n";
	serverLog(_clients[i].getNick(), "received string: [" + str + "]");
	send(_clients[i].getSocket(), reply.c_str(), reply.size(), 0);
}

void	Server::channelBroadcast(int i, std::string chName, std::string str)
{
	int chId = getChannelId(chName);
	for (int j = 0; j < _clients.size(); j++) {
		if (chName == _clients[j].getConnectedChannel(chId))
			sendToClient(j, str);
	}
}

void	Server::clientBroadcast(int i, std::string chName, std::string str)
{
	int chId = getChannelId(chName);
	for (int j = 0; j < _clients.size(); j++) {
		if (chName == _clients[j].getConnectedChannel(chId) && i != j)
			sendToClient(j, str);
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
	if (i == -1)
		return (false);
	for (std::map<int, std::string>::iterator it = _clients[i].getChannels().begin(); it != _clients[i].getChannels().end(); it++) {
		if (chId == it->first)
			return (true);
	}
	return (false);
}

