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

int		Server::findOrCreateChannel(int i, std::string name)
{
	std::string channelTarget = name.substr(0, name.find(' ', 0));


	for (int j = 0; j < _channels.size(); j++) {
		if (channelTarget == _channels[j].getName())
			return (_channels[j].getId());
	}
	_channels.push_back(Channel(channelTarget));
	_clients[i].setOp(true);
	std::cout << _channels.rbegin()->getName() << " has been created" << std::endl;
	return (_channels.rbegin()->getId());
}

void	Server::commandJoin(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())));

	//!parse: find the #; find if there is a key, and test it

	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "JOIN")));
	if (args[0] != '#')
		return (sendToClient(i, ERR_BADCHANMASK(_clients[i].getNick(), args)));

	std::string chName, key;
	setJoin(args, &chName, &key);
	
	int channelId = findOrCreateChannel(i, args);
	if (key != _channels[channelId].getChannelKey())
		return (sendToClient(i, ERR_BADCHANNELKEY(_clients[i].getNick(), chName)));
	if (_channels[channelId].getNbrClients() >= _channels[channelId].getLimit() && _channels[channelId].getLimit() != 0)
		return (sendToClient(i, ERR_CHANNELISFULL(_clients[i].getNick(), chName)));
	if (_channels[channelId].isInviteOnly())
		return (sendToClient(i, ERR_INVITEONLYCHAN(_clients[i].getNick(), chName)));


	_clients[i].setChannelId(channelId);
	_clients[i].setchannelName( _channels[_clients[i].getChannelId()].getName());//!does client really need the name?
	_channels[_clients[i].getChannelId()].incrementNbrClients();

	sendToClient(i, "You have joined channel " + chName);

	//outputs
//RPL_TOPIC
//RPL_NAMREPLAY
//send list of names
//RPL_JOIN for everyone
	//*OTHER MEMBERS OF CHANNEL KNOWING CLIENT JOINED
	// std::string strToSend = nick + " joined " + _clients[i].getChannelName();
	// sendToClientsInChannel(i, strToSend);
	//if (topic != empty)
	// sendToClient(i, RPL_TOPIC(nick, chName, "TEMP TOPIC"));
}