#include "../includes/Server.hpp"
//todo PARSING
//parse QUIT, quit can get a <reason>, //!FIX QUIT OUTPUT


bool	Server::isValidPart(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "PART")), false);
	return (true);
}

void	Server::leaveChannel(int i, int chId)
{
	_channels[chId].removeClient(i);
	_clients[i].getChannels().erase(chId);
}

void	Server::commandPart(int i, std::string args)
{
	if (!isValidPart(i, args))
		return ;
	int chId = getChannelId(args);
	if (chId == -1)
		return (sendToClient(i, ERR_NOSUCHCHANNEL(args)));
	if (!isUserInChannel(i, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), args)));

	std::string strToSend = _clients[i].getPrefix() + " PART " + args;
	channelBroadcast(chId, strToSend);
	leaveChannel(i, chId);
}


void	Server::commandQuit(int i, std::string args)
{
	serverLog(_clients[i].getNick(), "has disconnected");
	std::string strToSend = _clients[i].getPrefix() + " QUIT :Quit: " + args;
	serverBroadcast(strToSend);

	for (std::map<int, std::string>::iterator it = _clients[i].getChannels().begin(); 
		it != _clients[i].getChannels().end(); it++)
			leaveChannel(i, it->first);

	close(i);
	_clients.erase(i);
}