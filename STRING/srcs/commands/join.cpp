#include "../includes/Server.hpp"

//todo DOUBLE CHECK OUTPUTS AND PARSING
//if needed, created a hardcoded channel with a key to check everything is well created

void	setJoin(std::string args, std::string *chName, std::string *key)
{
	int pos = args.find(' ');
	*chName = args.substr(0, pos);
	if (pos != std::string::npos)
		*key = args.substr(pos + 1);
}

int		Server::findOrCreateChannel(int i, std::string chName)
{
	for (int j = 0; j < _channels.size(); j++) {
		if (chName == _channels[j].getName())
			return (_channels[j].getId());//*Returns found channel
	}
	//*Creating channel
	_channels.push_back(Channel(chName));
	int chId = _channels.rbegin()->getId();
	_channels[chId].setOp(_clients[i].getId(), true);
	std::cout << _channels.rbegin()->getName() << " has been created" << std::endl;//idk if this output is supposed to be here
	return (chId);
}

//todo parse: find if there is a key, and test it
void	Server::commandJoin(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())));
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "JOIN")));
	// if (args[0] != '#')
	// 	return (sendToClient(i, ERR_BADCHANMASK(_clients[i].getNick(), args)));

	std::string chName, key;
	setJoin(args, &chName, &key);
	
	//*Errors
	int channelId = findOrCreateChannel(i, chName);//can use chName as parameter and directly compare
	if (key != _channels[channelId].getChannelKey())
		return (sendToClient(i, ERR_BADCHANNELKEY(_clients[i].getNick(), chName)));
	if (_channels[channelId].getNbrClients() >= _channels[channelId].getLimit() && _channels[channelId].getLimit() != 0)
		return (sendToClient(i, ERR_CHANNELISFULL(_clients[i].getNick(), chName)));
	if (_channels[channelId].isInviteOnly())
		return (sendToClient(i, ERR_INVITEONLYCHAN(_clients[i].getNick(), chName)));
	//already joined error?

	//*Joins the channel
	_clients[i].setChannel(channelId, chName);
	_channels[_clients[i].getChannelIdNew(chName)].incrementNbrClients();
	std::string strToSend = _clients[i].getPrefix() + " JOIN " + chName;
	serverBroadcast(i, chName, strToSend);//I THINK ITS SERVER BROADCAST, IDK IF CLIENT JOINING HAS A DIFFERENT OUTPUT
}