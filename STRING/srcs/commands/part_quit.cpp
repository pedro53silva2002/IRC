#include "../includes/Server.hpp"
//todo CHECK OUTPUTS, AND REDO QUIT



//!MAKE
/*
void	Server::leaveChannel(int i, int chId)
{

}

void	Server::commandPart(int i, std::string name)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())));
	if (name.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "PART")));

	if (!hasInChannels(name))
		return (sendToClient(i, ERR_NOSUCHCHANNEL(_clients[i].getNick(), name)));
	
	sendToClient(i, "You have left channel " + name);//todo check the actual output
	std::string strToSend = _clients[i].getNick() + " left " + name;
	int chId = _clients[i].getChannelIdNew(name);
	clientBroadcast(i, _clients[i].getChannelNameNew(chId), strToSend);
	_channels[chId].decrementNbrClients();
	leaveChannel(i, chId);
}
*/

//*QUIT

//calling QUIT asks for reason, ctrl+c doesnt need reason
void	Server::commandQuit(int i, std::string str)
{
	serverLog(_clients[i].getNick(), "has disconnected");
	sendToClient(i, "QUIT :" + str);
	// for (std::map<int, std::string>::iterator it = _clients[i].getChannels().begin(); it != _clients[i].getChannels().end(); i++) {
	// 	_channels[it->first].decrementNbrClients();
	// }
	close (_pfds[i].fd);
	_clients.erase(_clients.begin() + i);
}