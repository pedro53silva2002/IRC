#include "../includes/Server.hpp"

//todo REDOOOOOOOOOOOOOOOOOOOOO
//outputs, numerics, parsing

void	Server::noArgsTopic(int i)
{
	if (_channels[_clients[i].getChannelId()].getTopic().empty())//	wtf...
		return (sendToClient(i, RPL_NOTOPIC(_clients[i].getNick(), _channels[_clients[i].getChannelId()].getName())));//todo check output
	return (sendToClient(i, RPL_TOPIC(_clients[i].getNick(), _channels[_clients[i].getChannelId()].getName(),  _channels[_clients[i].getChannelId()].getTopic())));//todo check output
}

//CHANOPRIVSNEEDED

//broken if client has left the channel (mightve been fixed now with the quit and part changes)
void	Server::commandTopic(int i, std::string line)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())));
		//TODO HAVE A FUNCTION THAT PARSES THIS COMMAND
	if (line.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "TOPIC")));
	//todo parse
	/*
		TOPIC <channel> [<topic>]
		if <topic> is not given, RPL_TOPIC or RPL_NOTOPIC
		else if topicRestricted, only op can edit topic
		else user edits topic

		valid:
		TOPIC #channel :<topic
		TOPIC #channel :					(clears channel)
		TOPIC #cannel
	*/
	
	size_t pos = line.find(' ', 0);
	std::string channelTarget = line.substr(0, pos);
	if (!hasInUserChannels(_clients[i], channelTarget))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), channelTarget)));//todo check output
	
	std::string topic = line.substr(pos + 1);
	if (pos == std::string::npos || topic.empty())
		noArgsTopic(i);
	else 
		std::cout << "SET TOPIC\n";
	for (int j = 0; j < _channels.size(); j++) {
		if (_channels[j].getName() == channelTarget) {
			if (_channels[j].isTopicRestricted() && _clients[i].getOp())
				return (sendToClient(i, ERR_CHANOPRIVSNEEDED(_clients[i].getNick(), _channels[j].getName())));
			_channels[i].setTopic(topic);
			sendToClient(i, RPL_TOPIC(_clients[i].getNick(), _channels[j].getName(), topic));
		}
	}
}