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

/**
 * @brief Validates the TOPIC command arguments before execution.
 * 
 * Performs basic validation to ensure the client is registered and
 * that a channel name was provided.
 * 
 * @param i    The file descriptor index of the client in the _clients map.
 * @param args The raw arguments: "<channel> [:<topic>]".
 * 
 * @return true if basic validation passes.
 * @return false if validation fails (error sent to client).
 * 
 * @note Sends ERR_NOTREGISTERED (451) if client is not registered.
 * @note Sends ERR_NEEDMOREPARAMS (461) if no arguments provided.
 */
bool	Server::isValidTopic(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "TOPIC")), false);
	return (true);
}

/**
 * @brief Parses TOPIC command arguments into channel and topic components.
 * 
 * Extracts the channel name and optional new topic from the raw TOPIC
 * command arguments string.
 * 
 * @param line     The raw arguments string in format "<channel> [:<topic>]".
 * @param channel  Output pointer to store the extracted channel name.
 * @param newTopic Output pointer to store the new topic (empty if not provided).
 * 
 * @note If no topic is provided, newTopic is set to an empty string.
 */
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

/**
 * @brief Sends the current channel topic to the client.
 * 
 * Called when TOPIC command is used without a new topic argument.
 * Returns the current topic or indicates that no topic is set.
 * 
 * @param i      The file descriptor index of the client in the _clients map.
 * @param chName The name of the channel to query.
 * 
 * @note Sends RPL_NOTOPIC (331) if the channel has no topic set.
 * @note Sends RPL_TOPIC (332) with the current topic if one exists.
 */
void	Server::noArgsTopic(int i, std::string chName)
{
	int chId = getChannelId(chName);
	if (_channels[chId].getTopic().empty())
		return (sendToClient(i, RPL_NOTOPIC(_clients[i].getNick(), chName)));
	return (sendToClient(i, RPL_TOPIC(_clients[i].getNick(), chName, _channels[chId].getTopic())));
}

/**
 * @brief Handles the TOPIC command to view or set a channel's topic.
 * 
 * If no topic argument is provided, displays the current topic.
 * If a topic is provided, sets the new topic (requires operator
 * privileges when topic restriction mode is enabled).
 * 
 * @param i    The file descriptor index of the client in the _clients map.
 * @param args The raw arguments: "<channel> [:<topic>]".
 * 
 * @note Sends ERR_NOTONCHANNEL (442) if client is not in the channel.
 * @note Sends ERR_CHANOPRIVSNEEDED (482) if topic is restricted and client is not op.
 * @note Broadcasts new topic to all channel members when changed.
 * 
 * @see isValidTopic() for basic validation (registration, params).
 * @see setTopicArgs() for parsing channel and topic.
 * @see noArgsTopic() for displaying the current topic.
 * 
 */
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