#include "../includes/Server.hpp"

bool	Server::isValidTopic(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "TOPIC")), false);
	return (true);
}

void	setTopicArgs(std::string line, std::string *channel, std::string *newTopic)
{
	int pos = line.find(' ');
	*channel = line.substr(0, pos);
	if (pos != std::string::npos) {
		std::string rest = line.substr(pos + 1);
		*newTopic = rest;
	}
	else
		*newTopic = "";
}

void	Server::noArgsTopic(int i, std::string chName)
{
	int chId = getChannelId(chName);
	if (_channels[chId].getTopic().empty())
		return (sendToClient(i, RPL_NOTOPIC(_clients[i].getNick(), chName)));
	return (sendToClient(i, RPL_TOPIC(_clients[i].getNick(), chName, _channels[chId].getTopic())));
}

void	Server::commandTopic(int i, std::string args)
{
	if (!isValidTopic(i, args))
		return ;

	std::string chName, newTopic;
	setTopicArgs(args, &chName, &newTopic);

	int chId = getChannelId(chName);
	if (!isUserInChannel(i, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));
	if (newTopic.empty())
		return (noArgsTopic(i, chName));
	if (_channels[chId].isTopicRestricted() && !_channels[chId].isOp(_clients[i].getId()))
		return (sendToClient(i, ERR_CHANOPRIVSNEEDED(_clients[i].getNick(), chName)));

	_channels[chId].setTopic(newTopic);
	channelBroadcast(chId, RPL_TOPIC(_clients[i].getNick(), chName, newTopic));
}