#include "../includes/Server.hpp"

bool	Server::isValidPrivmsg(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "PRIVMSG")), false);
	
	size_t pos = args.find(' ');
	if (pos == std::string::npos || args.substr(pos + 1).empty())
		return (false);
	return (true);
}

void	setPrivmsg(std::string args, std::string *target, std::string *message)
{
	size_t pos = args.find(' ');
	*target = args.substr(0, pos);
	std::string rest = args.substr(pos + 1);
	if (rest[0] == ':')
		*message = rest.substr(1);
	else
		*message = rest.substr(0, rest.find(' '));
	std::transform((*target).begin(), (*target).end(), (*target).begin(), ::tolower);
}

void	Server::privmsgChannel(int i, std::string chName, std::string message) {
	int chId = getChannelId(chName);
	if (!isUserInChannel(i, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));
	clientBroadcast(i, chId, PRIVMSG(_clients[i].getNick(), chName, message));
}

void	Server::privmsgUser(int i, std::string clientName, std::string message) {
	int clientId = getClientId(clientName);
	if (clientId == -1)
		return (sendToClient(i, ERR_NOSUCHNICK(_clients[i].getNick(), clientName)));
	sendToClient(clientId, PRIVMSG(_clients[i].getNick(), clientName, message));
}

void	Server::commandPrivmsg(int i, std::string args)
{
	if (!isValidPrivmsg(i, args))
		return ;
	
	std::string targetName, message;
	setPrivmsg(args, &targetName, &message);

	if (targetName[0] == '#')
		privmsgChannel(i, targetName, message);
	else
		privmsgUser(i, targetName, message);
}
