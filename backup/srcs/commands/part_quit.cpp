#include "../includes/Server.hpp"
//todo CHECK OUTPUTS, AND REDO QUIT

void	Server::commandPart(int i, std::string name)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())));
	if (name.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "PART")));

	if (!hasInChannels(name))//remake this function
		return (sendToClient(i, ERR_NOSUCHCHANNEL(_clients[i].getNick(), name)));
	
	sendToClient(i, "You have left channel " + name);//!check the actual output
	std::string strToSend = _clients[i].getNick() + " left " + name;
	sendToClientsInChannel(i, strToSend);
	_channels[_clients[i].getChannelId()].decrementNbrClients();
	_clients[i].setChannelId(-1);
	_clients[i].setchannelName("");
}

//*Disconnect client when client exits
void	Server::commandQuit(int i, std::string str)
{
	serverLog(_clients[i].getNick(), "has disconnected");
	//calling QUIT asks for reason, ctrl+c doesnt need reason
	sendToClient(i, "QUIT :" + str);
	_channels[_clients[i].getChannelId()].decrementNbrClients();
	close (_pfds[i].fd);
	_clients.erase(_clients.begin() + i);
}