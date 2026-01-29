#include "../includes/Server.hpp"

bool	Server::isValidTopic(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "TOPIC")), false);
	return (true);
}

void	setTopicArgs(std::string line, std::string *chName, std::string *newTopic)
{
	size_t pos = line.find(' ');
	*chName = line.substr(0, pos);
	if (pos != std::string::npos) {
		std::string rest = line.substr(pos + 1);
		*newTopic = rest;
	}
	else
		*newTopic = "";
}

//erase topic needs to be checked
void	Server::commandTopic(int i, std::string args)
{
	if (!isValidTopic(i, args))
		return ;

	std::string chName, newTopic;
	setTopicArgs(args, &chName, &newTopic);

	int chId = getChannelId(chName);
	if (!isUserInChannel(i, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));
	if (newTopic.empty()) {
		if (_channels[chId].getTopic().empty() || _channels[chId].getTopic() == ":")
			return (sendToClient(i, RPL_NOTOPIC(_clients[i].getNick(), chName)));
		sendToClient(i, RPL_TOPIC( _clients[i].getNick(), chName, _channels[chId].getTopic()));
		sendToClient(i, RPL_TOPICWHOTIME(_clients[i].getPrefix(), chName, _channels[chId].getTopicAuthor(), _channels[chId].getTopicTimeSet()));
		return ;
	}
	if (_channels[chId].isTopicRestricted() && !_channels[chId].isOp(i))
		return (sendToClient(i, ERR_CHANOPRIVSNEEDED(_clients[i].getNick(), chName)));

	_channels[chId].setTopic(newTopic);
	_channels[chId].setTopicTimeSet();
	_channels[chId].setTopicAuthor(_clients[i].getPrefix());
	// channelBroadcast(chId, RPL_TOPIC(_clients[i].getNick(), chName, newTopic));
	//CHANGE THIS TO A RPLY
	channelBroadcast(chId, _clients[i].getPrefix() + " TOPIC " + chName + " :" + newTopic);
}
