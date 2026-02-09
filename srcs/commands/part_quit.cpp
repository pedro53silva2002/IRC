#include "../includes/Server.hpp"

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
	_clients[i].getChannels().erase(chId);
	_channels[chId].removeClient(i);
	if (_channels[chId].getClientsInChannel().empty()) {
		serverLog(_channels[chId].getName(), "is empty, deleting");
		_channels.erase(_channels.begin() + chId);
		_channels[chId].decrementId();
	}
}

void	setPart(std::string args, std::string *chName, std::string *reason)
{
	size_t pos = args.find(' ');
	*chName = args.substr(0, pos);
	*reason = args.substr(pos + 1);
	std::transform((*chName).begin(), (*chName).end(), (*chName).begin(), ::tolower);
}

void	Server::commandPart(int i, std::string args)
{
	if (!isValidPart(i, args))
		return ;
	std::string chName, reason;
	setPart(args, &chName, &reason);
	
	int chId = getChannelId(chName);
	if (!isUserInChannel(i, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));
	channelBroadcast(chId, PART(_clients[i].getNick(), chName));
	leaveChannel(i, chId);
}

void	Server::commandQuit(int i, std::string args)
{
	serverLog(_clients[i].getNick(), "has disconnected");
	std::string strToSend = _clients[i].getPrefix() + " QUIT :Quit: " + args;
	serverBroadcast(strToSend);

	std::map<int, std::string> &chans = _clients[i].getChannels();
	for (std::map<int, std::string>::iterator it = chans.begin();  it != chans.end();) {
		int chId = it->first;
		++it;
		leaveChannel(i, chId);
	}

	close(_clients[i].getSocket());
	_clients.erase(i);
}