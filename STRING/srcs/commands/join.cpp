#include "../includes/Server.hpp"

//todo PARSING AND KEY

void	setJoin(std::string args, std::string *chName, std::string *key)
{
	int pos = args.find(' ');
	*chName = args.substr(0, pos);
	if (pos != std::string::npos)
		*key = args.substr(pos + 1);
}

int		Server::findOrCreateChannel(int i, std::string chName)
{
	for (int j = 0; j < _channels.size(); j++) {
		if (chName == _channels[j].getName())
			return (_channels[j].getId());
	}
	//setTopic to whatever
	_channels.push_back(Channel(chName));
	int chId = _channels.rbegin()->getId();
	_channels[chId].setOp(_clients[i].getId(), true);
	serverLog("channel created: ", _channels.rbegin()->getName());
	return (chId);
}

void	Server::commandJoin(int i, std::string args)
{
	if (args[0] != '#')
		return (sendToClient(i, ERR_BADCHANMASK(_clients[i].getNick(), args)));

	std::string chName, key;
	setJoin(args, &chName, &key);
	
	int chId = findOrCreateChannel(i, chName);
	if (key != _channels[chId].getChannelKey())
	//PSSWDMISMATCH instead?
		return (sendToClient(i, ERR_BADCHANNELKEY(_clients[i].getNick(), chName)));
	if (_channels[chId].getClientsInChannel().size() >= _channels[chId].getLimit() && _channels[chId].getLimit() != 0)
		return (sendToClient(i, ERR_CHANNELISFULL(_clients[i].getNick(), chName)));
	if (_channels[chId].isInviteOnly())
		return (sendToClient(i, ERR_INVITEONLYCHAN(_clients[i].getNick(), chName)));

	if (isUserInChannel(i, chId))
		return (sendToClient(i, ERR_USERONCHANNEL(_clients[i].getNick(), chName)));

	_clients[i].setChannel(chId, chName);
	// RPL_TOPIC2
	// RPL_NAMREPLY (create a user list)
	_channels[chId].addClient(i);
	std::string strToSend = _clients[i].getPrefix() + " JOIN " + chName;
	channelBroadcast(chId, strToSend);

}