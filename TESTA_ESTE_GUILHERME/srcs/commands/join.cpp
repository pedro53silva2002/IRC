#include "../includes/Server.hpp"

bool	Server::isValidJoin(int i, std::string args)
{
	if (!_clients[i].isRegistered()) 
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "JOIN")), false);
	if (args[0] != '#')
		return (sendToClient(i, ERR_BADCHANMASK(_clients[i].getNick(), args)), false);
	return (true);
}

void	setJoin(std::string args, std::string *chName, std::string *key)
{
	int pos = args.find(' ');
	*chName = args.substr(0, pos);
	if (pos != std::string::npos)
		*key = args.substr(pos + 1);
}

bool ischNameValid(std::string chName)
{
	for (int i = 0; i < chName.size(); i++) {
		char c = chName[i];
		if (c == 0x00 || c == 0x07 || c == 0x20 || c == 0x2C)
			return (false);
	}
	return (true);
}

int		Server::findOrCreateChannel(int i, std::string chName)
{
	for (int j = 0; j < _channels.size(); j++) {
		if (chName == _channels[j].getName())
			return (_channels[j].getId());
	}

	_channels.push_back(Channel(chName));
	int chId = _channels.rbegin()->getId();
	_channels[chId].setOp(i, true);
	_channels[chId].setTopic("general");
	serverLog("channel created: ", _channels.rbegin()->getName());
	return (chId);
}

void	Server::commandJoin(int i, std::string args)
{
	if (!isValidJoin(i, args))
		return ;

	std::string chName, key;
	setJoin(args, &chName, &key);
	if (!ischNameValid(chName))
		return (sendToClient(i, ERR_BADCHANMASK(_clients[i].getNick(), chName)));

	
	int chId = findOrCreateChannel(i, chName);
	if (key != _channels[chId].getChannelKey())
		return (sendToClient(i, ERR_BADCHANNELKEY(_clients[i].getNick(), chName)));
	if (_channels[chId].getClientsInChannel().size() >= _channels[chId].getLimit() && _channels[chId].getLimit() != 0)
		return (sendToClient(i, ERR_CHANNELISFULL(_clients[i].getNick(), chName)));
	if (_channels[chId].isInviteOnly())
		return (sendToClient(i, ERR_INVITEONLYCHAN(_clients[i].getNick(), chName)));

	if (isUserInChannel(i, chId))
		return (sendToClient(i, ERR_USERONCHANNEL(_clients[i].getNick(), chName)));

	_clients[i].setChannel(chId, chName);
	_channels[chId].addClient(i);
	std::string strToSend = _clients[i].getPrefix() + " JOIN " + chName;
	channelBroadcast(chId, strToSend);
	sendToClient(i, RPL_TOPIC(_clients[i].getNick(), chName, _channels[chId].getTopic()));
}