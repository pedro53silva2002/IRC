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

void	setKick(std::string line, std::string *chName, std::string *toKick)
{
	int pos = line.find(' ');
	*chName = line.substr(0, pos);
	*toKick = line.substr(pos + 1);
}

void	Server::commandKick(int i, std::string line)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())));
	if (line.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "KICK")));

	//! PARSE
	std::string chName, toKick;
	setKick(line, &chName, &toKick);
	int chId = _clients[i].getChannelIdNew(chName);
	if (chId == -1 || _clients[i].getChannelNameNew(chId) != chName)
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));
	if (!_channels[chId].isOp(_clients[i].getId()))
		return (sendToClient(i, ERR_NOPRIVILEGES(_clients[i].getNick())));
	if (toKick == _clients[i].getNick())
		return (sendToClient(i, " you cannot kick yourself FIX THIS STILL"));//!OUTPUT
	

	if (chId == toKick.getId())
//!aaaaaaaaaaaaaaaaaaaaaaaaaa
//!aaaaaaaaaaaaaaaaaaaaaaaaaa
//!aaaaaaaaaaaaaaaaaaaaaaaaaa
//!aaaaaaaaaaaaaaaaaaaaaaaaaa
//!aaaaaaaaaaaaaaaaaaaaaaaaaa
//!aaaaaaaaaaaaaaaaaaaaaaaaaa
//!aaaaaaaaaaaaaaaaaaaaaaaaaa
//TODO BASICALLY I CHANGED EVERYTHING TO CHANGE THE GETCHANNELID, GETCHANNELNAME, GETCLIENTID, GETCHANNELID FROM SERVER ITSELF WITH PARAMETERES






	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (toKick == it->getNick()) {
			//check toKick is in channel
			if (it->getChannelId() != _clients[i].getChannelId())
				return (sendToClient(i, ERR_USERNOTINCHANNEL(_clients[i].getNick(), toKick, chName)));
			clientBroadcast(i, _clients[i].getNick() + " KICK " + chName + " " + toKick);//todo + prefix
			it->setChannelId(-1);
			it->setchannelName("");
		}
	}
}