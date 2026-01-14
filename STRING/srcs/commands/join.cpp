#include "../includes/Server.hpp"

//todo DOUBLE CHECK OUTPUTS AND PARSING
//if needed, created a hardcoded channel with a key to check everything is well created

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
			return (_channels[j].getId());//*Returns found channel
	}
	//*Creating channel
	_channels.push_back(Channel(chName));
	int chId = _channels.rbegin()->getId();
	_channels[chId].setOp(_clients[i].getId(), true);
	serverLog("channel created: ", _channels.rbegin()->getName());
	return (chId);
}

//todo parse: find if there is a key, and test it
void	Server::commandJoin(int i, std::string args)
{
	// if (args[0] != '#')
	// 	return (sendToClient(i, ERR_BADCHANMASK(_clients[i].getNick(), args)));

	std::string chName, key;
	setJoin(args, &chName, &key);
	
	//*Errors
	int chId = findOrCreateChannel(i, chName);
	// if (key != _channels[chId].getChannelKey())
	// 	return (sendToClient(i, ERR_BADCHANNELKEY(_clients[i].getNick(), chName)));
	// if (_channels[chId].getNbrClients() >= _channels[chId].getLimit() && _channels[chId].getLimit() != 0)
	// 	return (sendToClient(i, ERR_CHANNELISFULL(_clients[i].getNick(), chName)));
	// if (_channels[chId].isInviteOnly())
	// 	return (sendToClient(i, ERR_INVITEONLYCHAN(_clients[i].getNick(), chName)));

	if (isUserInChannel(i, chId))
		return (sendToClient(i, "YOU ARE ALREADY IN THIS CHANNEL"));

	_clients[i].setChannel(chId, chName);
	_channels[chId].addClient(i);
	std::string strToSend = _clients[i].getPrefix() + " JOIN " + chName;
	channelBroadcast(chName, strToSend);
}