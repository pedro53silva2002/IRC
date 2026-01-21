#include "../includes/Server.hpp"
//todo PARSING, OUTPUT FOR KICKED CLIENT


bool	Server::isValidKick(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "KICK")), false);
	return (true);
}

void	setKick(std::string line, std::string *chName, std::string *toKickName)
{
	int pos = line.find(' ');
	*chName = line.substr(0, pos);
	*toKickName = line.substr(pos + 1);
}
//OPERATORS CAN KICK OTHER OPERATORS
void	Server::commandKick(int i, std::string args)
{
	if (!isValidKick(i, args))
		return ;
	std::string chName, toKickName;
	setKick(args, &chName, &toKickName);
	
	int chId = getChannelId(chName);
	int toKickId = getClientId(toKickName);
	if (!isUserInChannel(toKickId, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));
	if (!_channels[chId].isOp(i))
		return (sendToClient(i, ERR_NOPRIVILEGES(_clients[i].getNick())));
	if (toKickId == i)
		return (sendToClient(i, " you cannot kick yourself FIX THIS STILL"));
	
	leaveChannel(toKickId, chId);
	//todo output for kicked client

	std::string strToSend = _clients[i].getPrefix() + " KICK " + chName + " " + toKickName;
	clientBroadcast(i, chId, strToSend);
}