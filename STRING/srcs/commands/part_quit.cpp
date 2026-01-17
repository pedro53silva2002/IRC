#include "../includes/Server.hpp"
//todo PARSING, CHECK OUTPUTS, AND REDO QUIT
//parse QUIT, quit can get a <reason>, //!FIX QUICK OUTPUT


void	Server::leaveChannel(int i, int chId)
{
	_channels[chId].removeClient(i);
	_clients[i].getChannels().erase(chId);
}
void	Server::leaveAllChannels(int i)
{
	for (std::map<int, std::string>::iterator it = _clients[i].getChannels().begin(); 
		it != _clients[i].getChannels().end(); it++)
			leaveChannel(i, it->first);
}

void	Server::commandPart(int i, std::string name)
{
	int chId = getChannelId(name);
	if (chId == -1)
		return (sendToClient(i, ERR_NOSUCHCHANNEL(_clients[i].getNick(), name)));
	if (!isUserInChannel(i, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), name)));

		
	std::string strToSend = _clients[i].getPrefix() + " PART " + name;
	channelBroadcast(name, strToSend);

	leaveChannel(i, chId);
}


void	Server::commandQuit(int i, std::string str)
{
	serverLog(_clients[i].getNick(), "has disconnected");
	std::string strToSend = _clients[i].getPrefix() + " QUIT :Quit: " + str;
	serverBroadcast(strToSend);
	leaveAllChannels(i);
	close (i);
	_clients.erase(i);
}