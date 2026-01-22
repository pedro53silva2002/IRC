#include "../includes/Server.hpp"

bool	Server::isValidInvite(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "INVITE")), false);
	return (true);
}

void	setInvite(std::string line, std::string *invitedName, std::string *chName)
{
	int pos = line.find(' ');
	*invitedName = line.substr(0, pos);
	*chName = line.substr(pos + 1);
}

void	Server::commandInvite(int i, std::string args)
{
	if (!isValidInvite(i, args))
		return ;
	
	std::string invitedName, chName;
	setInvite(args, &invitedName, &chName);

	int chId = getChannelId(chName);
	if (!isUserInChannel(i, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));
	if (!_channels[chId].isOp(i) && _channels[chId].isInviteOnly())
		return (sendToClient(i, ERR_CHANOPRIVSNEEDED(_clients[i].getNick(), chName)));

	int invitedId = getClientId(invitedName);
	if (isUserInChannel(invitedId, chId))
		return (sendToClient(invitedId, ERR_USERONCHANNEL(_clients[invitedId].getNick(), chName)));

	_clients[invitedId].setChannel(chId, chName);
	_channels[chId].addClient(invitedId);
	std::string strToSend = _clients[i].getPrefix() + " INVITE " + invitedName + " " + chName;
	channelBroadcast(chId, strToSend);
	sendToClient(invitedId, RPL_TOPIC(_clients[invitedId].getNick(), chName, _channels[chId].getTopic()));
	sendToClient(i, RPL_INVITING(invitedName, _clients[i].getNick(), chName));
}
