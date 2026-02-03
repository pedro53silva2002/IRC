#include "../includes/Server.hpp"

void	setInvite(std::string line, std::string *invitedName, std::string *chName)
{
	int pos = line.find(' ');
	*invitedName = line.substr(0, pos);
	*chName = line.substr(pos + 1);
}

void	Server::commandInvite(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())));
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "INVITE")));
	
	std::string invitedName, chName;
	setInvite(args, &invitedName, &chName);

	int chId = getChannelId(chName);
	if (!isUserInChannel(i, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));
	if (!_channels[chId].isOp(i))
		return (sendToClient(i, ERR_CHANOPRIVSNEEDED(_clients[i].getNick(), chName)));

	int invitedId = getClientId(invitedName);
	if (invitedId == -1)
		return (sendToClient(i, ERR_NOSUCHNICK(_clients[i].getNick(), invitedName)));
	if (isUserInChannel(invitedId, chId))
		return (sendToClient(invitedId, ERR_USERONCHANNEL(_clients[invitedId].getNick(), chName)));

	_channels[chId].addInvited(invitedId);
	sendToClient(i, RPL_INVITING(_clients[i].getPrefix(), invitedName, chName));
	sendToClient(invitedId, INVITE(_clients[i].getNick(), invitedName, chName));
}
