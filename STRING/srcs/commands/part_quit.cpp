#include "../includes/Server.hpp"
//todo PARSING, CHECK OUTPUTS, AND REDO QUIT


void	Server::commandPart(int i, std::string name)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())));
	if (name.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "PART")));
	
	int chId = getChannelId(name);
	if (chId == -1)
		return (sendToClient(i, ERR_NOSUCHCHANNEL(_clients[i].getNick(), name)));
	
	_channels[chId].removeClient(i);

	//TODO OUTPUTS
	//TODO BROADCASTS
	sendToClient(i, "You have left channel " + name);//todo check the actual output
	std::string strToSend = _clients[i].getNick() + " left " + name;
	clientBroadcast(i, getChannelName(chId), strToSend);
}


//*QUIT
void	Server::leaveAllChannels(int i)
{
	for (std::map<int, std::string>::iterator it = _clients[i].getChannels().begin(); 
		it != _clients[i].getChannels().end(); it++) {
			_channels[it->first].removeClient(i);
		}
}

//calling QUIT asks for reason, ctrl+c doesnt need reason
void	Server::commandQuit(int i, std::string str)
{
	serverLog(_clients[i].getNick(), "has disconnected");
	sendToClient(i, "QUIT :" + str);
	leaveAllChannels(i);
	// for (std::map<int, std::string>::iterator it = _clients[i].getChannels().begin(); it != _clients[i].getChannels().end(); i++) {
	// 	_channels[it->first].decrementNbrClients();
	// }
	close (i);
	_clients.erase(i);
}