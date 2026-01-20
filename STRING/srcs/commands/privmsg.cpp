#include "../includes/Server.hpp"
//todo DO PRIVATE MESSAGES

/*
	check this output: 
	PRIVMSG #CHANNEL :
*/

bool	Server::isValidPrivmsg(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "PRIVMSG")), false);
	
	int pos = args.find(' ');
	if (pos == std::string::npos || args.substr(pos + 1).empty())
		return (false);
	return (true);
}

void	setPrivmsg(std::string args, std::string *channel, std::string *message)
{
	int pos = args.find(' ');
	*channel = args.substr(0, pos);
	std::string rest = args.substr(pos + 1);
	if (rest[0] == ':')
		*message = rest.substr(1);
	else
		*message = rest.substr(0, rest.find(' '));
}

void	Server::commandPrivmsg(int i, std::string args)
{
	if (!isValidPrivmsg(i, args))
		return ;
	
	std::string chName, message;
	setPrivmsg(args, &chName, &message);

	int chId = getChannelId(chName);
	if (!isUserInChannel(i, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));
	
	std::string toSend = _clients[i].getPrefix() + " PRIVMSG " + chName + " :" + message;
	clientBroadcast(i, chName, toSend);
}
