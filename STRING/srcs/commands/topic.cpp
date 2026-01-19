#include "../includes/Server.hpp"

//todo parse
/*
	TOPIC <channel> [<topic>]
	if <topic> is not given, RPL_TOPIC or RPL_NOTOPIC
	else if topicRestricted, only op can edit topic
	else user edits topic

	valid:
	TOPIC #channel :<topic?>
	TOPIC #channel :					(clears channel)//todo
	TOPIC #channel
	TOPIC #channel <something>			(does it ignore?)//todo
*/

//!TOPIC IS BROKEN
void	setTopicArgs(std::string line, std::string *channel, std::string *newTopic)
{
	int pos = line.find(' ');
	*channel = line.substr(0, pos);
	std::string rest = line.substr(pos + 1);
	*newTopic = rest;
}

void	Server::noArgsTopic(int i, std::string chName)
{
	int chId = getChannelId(chName);
	if (_channels[chId].getTopic().empty())
		return (sendToClient(i, RPL_NOTOPIC(_clients[i].getNick(), chName)));
	return (sendToClient(i, RPL_TOPIC(_clients[i].getNick(), chName, _channels[chId].getTopic())));
}

void	Server::commandTopic(int i, std::string line)
{	
	std::string chName, newTopic;
	setTopicArgs(line, &chName, &newTopic);

	int chId = getChannelId(chName);
	if (chId == -1 || !isUserInChannel(i, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));
	
	if (newTopic.empty())
		return (noArgsTopic(i, chName));

	if (_channels[chId].isTopicRestricted() && !_channels[chId].isOp(_clients[i].getId()))
		return (sendToClient(i, ERR_CHANOPRIVSNEEDED(_clients[i].getNick(), chName)));

	_channels[chId].setTopic(newTopic);
	channelBroadcast(chName, RPL_TOPIC(_clients[i].getNick(), chName, newTopic));
}