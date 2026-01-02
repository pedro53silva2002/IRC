#include "../includes/Server.hpp"

//todo REDOOOOOOOOOOOOOOOOOOOOOOOOOO

//ERR_USERONCHANNEL
//ERR_CHANNELISFULL, +l
void	Server::commandInvite(int i, std::string name)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())));
	//TODO HAVE A FUNCTION THAT PARSES THIS COMMAND
	if (name.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "INVITE")));

	
	std::string userToInvite = name.substr(0, name.find(' ', 0));
	std::string channelToGet = name.substr(name.find(' ', 0) + 1);
	int index;
	size_t pos = 0;
	int UserToInviteId;

	//todo pus isto em comentario, tenho de resolver depois
	// if (_clients[i].getChannelId() == -1) {
	// 	std::cout << _clients[i].getNick() << " cannot invite users without being in any channel." << std::endl;
	// 	sendToClient(_clients[i].getId(), "you cannot users without being in any channel");//!check the actual output
	// 	return ;
	// }
	if (findChannel(_clients[i], _channels, name, userToInvite, channelToGet))
		std::cout << "INVITED: " << userToInvite << " by " << _clients[i].getNick() << std::endl;
	for (size_t i = 1; i < _clients.size(); i++)
	{
		if (userToInvite == _clients[i].getNick())
			UserToInviteId = i;
	}
	for (std::vector<Channel>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); ++channelIt)
	{
		if (channelToGet == channelIt->getName())//change to use subster of the getName
		{
			index = channelIt - _channels.begin();
			break ;
		}
	}
	/* else
		std::cout << "Didnt found the channel to invite " << std::endl; */
	//std::cout << "USER TO INVITE: " << userToInvite << "(" << _clients[UserToInviteId].getNick() << ")\t" << " CHANNEL TO GET: " << channelToGet << std::endl;
	//std::cout << "CHANNEL TO INVITE: " << index << std::endl;
	if (_channels[index].getChannelKey() != "")
		commandJoin(UserToInviteId, (channelToGet + " " + _channels[index].getChannelKey()));
	else
		commandJoin(UserToInviteId, (channelToGet));
	//commandJoin(UserToInviteId, (channelToGet + " " + _channels[index].getChannelKey()));
}
