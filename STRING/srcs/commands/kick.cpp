#include "../includes/Server.hpp"
//todo REDOOOOOOOOOOOOOOOOOOOOOOOOO

//NEEDMOREPARAMS CAUSE YOU NEED "PRIVMSG <client> <channel>", check order
void	Server::commandKick(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())));
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "KICK")));

	if (!_channels[_clients[i].getChannelId()].getOp(_clients[i].getId()))
		return (sendToClient(i, ERR_NOPRIVILEGES(_clients[i].getNick())));

	

	std::string chName = args.substr(0, args.find(' '));//!PARSE
	std::string toKick = args.substr(args.find(' ') + 1);//!PARSE
	// std::cout << RED("channel name: ") << chName << RED(", toKick name: ") << toKick << std::endl;
	if (toKick == _clients[i].getNick())
		return (sendToClient(i, " you cannot kick yourself FIX THIS STILL"));//todo fix output

	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (toKick == it->getNick()) {
			//check toKick is in channel
			if (it->getChannelId() != _clients[i].getChannelId())
				return (sendToClient(i, ERR_USERNOTINCHANNEL(_clients[i].getNick(), toKick, chName)));
			clientBroadcast(i, _clients[i].getNick() + " KICK " + chName + " " + toKick);//todo + prefix
			it->setChannelId(-1);
			it->setchannelName("");
		}
	}
}