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
	std::string reply = str + "\r\n";//todo maybe its just \n
	serverLog(_clients[i].getNick(), "received string: [" + str + "]");
	send(_clients[i].getSocket(), reply.c_str(), reply.size(), 0);
}

//only difference is it also sends to current user
void	Server::serverBroadcast(int i, std::string chName, std::string str)
{
	int chId = _clients[i].getChannelIdNew(chName);
	for (int j = 0; j < _clients.size(); j++) {
		if (chId == _clients[j].getChannelIdNew(chName))
			sendToClient(j, str);
	}
}
void	Server::clientBroadcast(int i, std::string chName, std::string str)
{
	int chId = _clients[i].getChannelIdNew(chName);
	for (int j = 0; j < _clients.size(); j++) {
		if (chId == _clients[j].getChannelIdNew(chName) && i != j)
			sendToClient(j, str);
	}
}


//!BRUNO

int Server::getIdFromClients(std::string name)
{
	for (size_t i = 1; i < _clients.size(); i++)
	{
		if (name == _clients[i].getNick())
			return (i);
	}
	return (-1);
}
int Server::getIdFromChannels(std::string name)
{
	for (size_t i = 1; i < _channels.size(); i++)
	{
		if (name == _channels[i].getName())
			return (i);
	}
	return (-1);
}

bool Server::isUserInChannel(int clId, std::string chName)
{
	if (clId == -1)
		return (false);
	for (std::map<int, std::string>::iterator it = _clients[clId].getChannels().begin(); it != _clients[clId].getChannels().end(); it++) {
		if (chName == it->second)
			return (true);
	}
	return (false);
}


//!BRUNO
//* CHANNEL LOGIC
// bool Server::hasInChannels(std::string name)
// {
// 	for (int i = 0; i < _channels.size(); i++) {
// 		if (name == _channels[i].getName())
// 			return (true);
// 	}
// 	return (false);
// }

// int Server::findChannel(Client client, std::vector<Channel> channels,std::string name, std::string userToInvite, std::string channelToGet)
// {
// 	Client *invitedClient;
// 	invitedClient = foundInUsers(userToInvite);
// 	if (!invitedClient)
// 	{
// 		std::cout <<"User to invite was not found in the list of users" << std::endl;
// 		return (0);
// 	}
// 	else if (!hasInChannels(channelToGet))
// 	{
// 		std::cout <<"This channel was not found int the list of channels" << std::endl;
// 		return (0);
// 	}
// 	else if (!hasInUserChannels(client, channelToGet))
// 	{
// 		std::cout <<"This client is not in the channel" << std::endl;
// 		return (0);
// 	}
// 	else if (hasInUserChannels(*invitedClient, channelToGet))
// 	{
// 		std::cout <<"This invited client is already in the channel" << std::endl;
// 		return (0);
// 	}
// 	return (1);
// }

