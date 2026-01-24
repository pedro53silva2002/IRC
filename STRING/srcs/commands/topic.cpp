#include "../includes/Server.hpp"

/**
 * @brief Validates the TOPIC command arguments before execution.
 * 
 * Performs basic validation to ensure the client is registered
 * and that arguments were provided.
 * 
 * @param i    The file descriptor index of the client sending the command.
 * @param args The raw arguments: "<channel> [<topic>]".
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
 * @brief Parses the TOPIC command arguments into channel name and new topic.
 * 
 * Splits the input line at the first space character to extract the
 * channel name and, if present, the new topic.
 * 
 * @param line      The raw arguments string: "<channel> [<topic>]".
 * @param channel   Pointer to store the extracted channel name.
 * @param newTopic  Pointer to store the extracted new topic (empty if not provided).
 * 
 * @note If no space is found, the entire line is treated as the channel name.
 */
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

/**
 * @brief Handles the TOPIC command when no new topic is provided.
 * 
 * Retrieves and sends the current topic of the specified channel
 * to the requesting client. If no topic is set, notifies the client accordingly.
 * 
 * @param i      The file descriptor index of the client requesting the topic.
 * @param chName The name of the channel whose topic is requested.
 * 
 * @note Sends RPL_NOTOPIC (331) if no topic is set for the channel.
 * @note Sends RPL_TOPIC (332) with the current topic if it exists.
 */
void	Server::noArgsTopic(int i, std::string chName)
{
	int chId = getChannelId(chName);
	if (_channels[chId].getTopic().empty())
		return (sendToClient(i, RPL_NOTOPIC(_clients[i].getNick(), chName)));
	return (sendToClient(i, RPL_TOPIC(_clients[i].getNick(), chName, _channels[chId].getTopic())));
}

/**
 * @brief Handles the IRC TOPIC command to set or get a channel's topic.
 * 
 * Validates the command, checks permissions, and if successful,
 * sets a new topic for the specified channel or retrieves the current topic.
 * 
 * @param i    The file descriptor index of the client sending the command.
 * @param args The raw arguments: "<channel> [<topic>]".
 * 
 * @note If no topic is provided, calls noArgsTopic() to send the current topic.
 * @note Sends ERR_NOTONCHANNEL (442) if the client is not on the channel.
 * @note Sends ERR_CHANOPRIVSNEEDED (482) if topic is restricted and client is not an operator.
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
	if (_channels[chId].isTopicRestricted() && !_channels[chId].isOp(i))
		return (sendToClient(i, ERR_CHANOPRIVSNEEDED(_clients[i].getNick(), chName)));

	_channels[chId].setTopic(newTopic);
	channelBroadcast(chId, RPL_TOPIC(_clients[i].getNick(), chName, newTopic));
}