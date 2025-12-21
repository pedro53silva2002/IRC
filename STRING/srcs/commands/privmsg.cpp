#include "../includes/Server.hpp"
//todo DOUBLE CHECK


//*mostly done
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
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())));
	if (line.empty() || !isValidPrivmsg(line))
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "PRIVMSG")));
	
	std::string channel, message;
	setPrivmsg(line, &channel, &message);
	if (_clients[i].getChannelId() == -1 || _clients[i].getChannelName() != channel)
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), channel)));
	std::string toSend = _clients[i].getPrefix() + " PRIVMSG " + channel + " :" + message;

	sendToClientsInChannel(i, toSend);
}
