#include "../includes/Server.hpp"
//todo DO PRIVATE MESSAGES

/*
	check this output: 
	PRIVMSG #CHANNEL :
*/

bool	Server::isValidPrivmsg(std::string line)
{
	int pos = line.find(' ');
	if (pos == std::string::npos || line.substr(pos + 1).empty())
		return (false);
	return (true);
}

void	setPrivmsg(std::string line, std::string *channel, std::string *message)
{
	int pos = line.find(' ');
	*channel = line.substr(0, pos);
	std::string rest = line.substr(pos + 1);
	if (rest[0] == ':')
		*message = rest.substr(1);
	else
		*message = rest.substr(0, rest.find(' '));
}

void	Server::commandPrivmsg(int i, std::string line)
{
	if (!isValidPrivmsg(line))
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "PRIVMSG")));
	
	std::string chName, message;
	setPrivmsg(line, &chName, &message);

	int chId = getChannelId(chName);
	if (!isUserInChannel(i, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));
	
	std::string toSend = _clients[i].getPrefix() + " PRIVMSG " + chName + " :" + message;
	clientBroadcast(i, chName, toSend);
}
