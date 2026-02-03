#include "../includes/Server.hpp"

void	setKick(std::string line, std::string *chName, std::string *toKickName)
{
	int pos = line.find(' ');
	*chName = line.substr(0, pos);
	*toKickName = line.substr(pos + 1);
}

void	Server::commandKick(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())));
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "KICK")));
	
	std::string chName, toKickName;
	setKick(args, &chName, &toKickName);

	int chId = getChannelId(chName);
	if (!isUserInChannel(i, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));
	if (!_channels[chId].isOp(i))
		return (sendToClient(i, ERR_NOPRIVILEGES(_clients[i].getNick())));
	
	int toKickId = getClientId(toKickName);
	if (toKickId == -1)
		return (sendToClient(i, ERR_NOSUCHNICK(_clients[i].getNick(), toKickName)));
	if (!isUserInChannel(toKickId, chId))
		return (sendToClient(i, ERR_USERNOTINCHANNEL(_clients[i].getNick(), _clients[toKickId].getNick(), chName)));
	
	channelBroadcast(chId, KICK(_clients[i].getNick(), chName, toKickName));
	leaveChannel(toKickId, chId);
}