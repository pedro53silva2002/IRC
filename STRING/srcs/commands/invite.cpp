#include "../includes/Server.hpp"

//todo redoing

//ERR_USERONCHANNEL
//ERR_CHANNELISFULL, +l
//TODO PARSE

void	setInvite(std::string line, std::string *invitedName, std::string *chName)
{
	int pos = line.find(' ');
	*invitedName = line.substr(0, pos);
	*chName = line.substr(pos + 1);
}

void	Server::commandInvite(int i, std::string args)
{
	std::string invitedName, chName;
	setInvite(args, &invitedName, &chName);

	int chId = getChannelId(chName);
	if (!isUserInChannel(i, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));

	int invitedId = getClientId(invitedName);
	if (isUserInChannel(invitedId, chId))
		return (sendToClient(i, "CLIENT ALREADY IN CHANNEL, CREATE OUTPUT"));

	//!check if key is needed in case its locked channel, prob not because user inviting the other is in channel
	_clients[invitedId].setChannel(chId, chName);
	_channels[chId].addClient(invitedId);
	std::string strToSend = _clients[i].getPrefix() + " INVITED " + invitedName + " TO THE CHANNEL, CHECK OUTPUT";
	channelBroadcast(chId, strToSend);
}
