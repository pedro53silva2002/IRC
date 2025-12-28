#include "../includes/Server.hpp"
//todo CHECK OUTPUTS, AND REDO QUIT

/* void	Server::commandPart(int i, std::string name)
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
	int chId = _clients[i].getChannelIdNew(name);


	_channels[chId].decrementNbrClients();
	_clients[i].setChannelId(-1);
	_clients[i].setchannelName("");
} */



//*QUIT
void	Server::leaveChannels(int i)//hopefully this works
{
	for (std::map<int, std::string>::iterator it = _clients[i].getChannels().begin(); it != _clients[i].getChannels().end(); i++) {
		_channels[it->first].decrementNbrClients();
	}
}

//calling QUIT asks for reason, ctrl+c doesnt need reason
void	Server::commandQuit(int i, std::string str)
{
	serverLog(_clients[i].getNick(), "has disconnected");
	sendToClient(i, "QUIT :" + str);
	leaveChannels(i);
	close (_pfds[i].fd);
	_clients.erase(_clients.begin() + i);
}