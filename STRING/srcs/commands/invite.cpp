#include "../includes/Server.hpp"

//ERR_USERONCHANNEL
//ERR_CHANNELISFULL, +l
//TODO PARSE

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
//CHANOPRIVSNEEDEDE if inviteOnly?
//RPL_INVITING to i
//INVITE message to invited user
void	Server::commandInvite(int i, std::string args)
{
	if (!isValidInvite(i, args))
		return ;
	
	std::string invitedName, chName;
	setInvite(args, &invitedName, &chName);

	int chId = getChannelId(chName);
	if (!isUserInChannel(i, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));

	int invitedId = getClientId(invitedName);
	//check if this output is the same
	if (isUserInChannel(invitedId, chId))
		return (sendToClient(invitedId, ERR_USERONCHANNEL(_clients[invitedId].getNick(), chName)));

	//!check if key is needed in case its locked channel, prob not because user inviting the other is in channel
	_clients[invitedId].setChannel(chId, chName);
	_channels[chId].addClient(invitedId);
	std::string strToSend = _clients[i].getPrefix() + " INVITED " + invitedName + " TO THE CHANNEL, CHECK OUTPUT";
	channelBroadcast(chId, strToSend);
}
