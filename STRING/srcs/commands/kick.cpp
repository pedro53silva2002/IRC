#include "../includes/Server.hpp"
//todo PARSING, OUTPUTS CHECK, BROADCAST


// bool	Server::isValidKick(std::string line)
// {
//		todo old
// 	int pos = line.find(' ');
// 	if (pos == std::string::npos || line.substr(pos + 1).empty())
// 		return (false);
// 	return (true);
// }

void	setKick(std::string line, std::string *chName, std::string *toKickName)
{
	int pos = line.find(' ');
	*chName = line.substr(0, pos);
	*toKickName = line.substr(pos + 1);
}

//todo replace _clients[i].getId() by i, check if its correct
void	Server::commandKick(int i, std::string line)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())));
	if (line.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "KICK")));

	//! PARSE
	std::string chName, toKickName;
	setKick(line, &chName, &toKickName);
	int chId = getChannelId(chName);
	int toKickId = getClientId(toKickName);
	if (chId == -1 || !isUserInChannel(toKickId, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));
	if (!_channels[chId].isOp(_clients[i].getId()))
		return (sendToClient(i, ERR_NOPRIVILEGES(_clients[i].getNick())));
	if (toKickId == _clients[i].getId())
		return (sendToClient(i, " you cannot kick yourself FIX THIS STILL"));
	
	leaveChannel(toKickId, chId);
	//TODO OUTPUTS
	//TODO BROADCASTS
}