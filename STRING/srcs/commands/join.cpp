#include "../includes/Server.hpp"

/**
 * @brief Validates the JOIN command arguments before execution.
 * 
 * Performs basic validation to ensure the client is registered,
 * arguments were provided, and the channel name starts with '#'.
 * 
 * @param i    The file descriptor index of the client sending the command.
 * @param args The raw arguments: "<channel> [key]".
 * 
 * @return true if basic validation passes.
 * @return false if validation fails (error sent to client).
 * 
 * @note Sends ERR_NOTREGISTERED (451) if client is not registered.
 * @note Sends ERR_NEEDMOREPARAMS (461) if no arguments provided.
 * @note Sends ERR_BADCHANMASK (476) if channel name doesn't start with '#'.
 */
bool	Server::isValidJoin(int i, std::string args)
{
	if (!_clients[i].isRegistered()) 
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "JOIN")), false);
	if (args[0] != '#')
		return (sendToClient(i, ERR_BADCHANMASK(_clients[i].getNick(), args)), false);
	return (true);
}

/**
 * @brief Parses the JOIN command arguments into channel name and optional key.
 * 
 * Splits the input arguments at the first space character to extract
 * the channel name and, if present, the channel key.
 * 
 * @param args   The raw arguments string: "<channel> [key]".
 * @param chName Pointer to store the extracted channel name.
 * @param key    Pointer to store the extracted key (unchanged if no key provided).
 * 
 * @note If no space is found, the entire args string is treated as the channel name.
 */
void	setJoin(std::string args, std::string *chName, std::string *key)
{
	size_t pos = args.find(' ');
	*chName = args.substr(0, pos);
	if (pos != std::string::npos)
		*key = args.substr(pos + 1);
}

/**
 * @brief Validates a channel name according to IRC specifications.
 * 
 * Checks that the channel name does not contain any forbidden characters
 * as defined by the IRC protocol.
 * 
 * @param chName The channel name to validate.
 * 
 * @return true if the channel name is valid.
 * @return false if the channel name contains forbidden characters.
 * 
 * @note Forbidden characters: NUL (0x00), BELL (0x07), SPACE (0x20), COMMA (0x2C).
 */
bool ischNameValid(std::string chName)
{
	for (size_t i = 0; i < chName.size(); i++) {
		char c = chName[i];
		if (c == 0x00 || c == 0x07 || c == 0x20 || c == 0x2C)
			return (false);
	}
	return (true);
}

/**
 * @brief Finds an existing channel or creates a new one.
 * 
 * Searches for a channel by name. If found, returns its ID.
 * If not found, creates a new channel with the given name,
 * sets the requesting client as operator, and assigns a default topic.
 * 
 * @param i      The file descriptor index of the client requesting the channel.
 * @param chName The name of the channel to find or create.
 * 
 * @return The ID of the existing or newly created channel.
 * 
 * @note If a new channel is created, the requesting client becomes an operator.
 * @note New channels are initialized with the default topic "general".
 * @note Logs channel creation to the server log.
 */
int		Server::findOrCreateChannel(int i, std::string chName)
{
	for (size_t j = 0; j < _channels.size(); j++) {
		if (chName == _channels[j].getName())
			return (_channels[j].getId());
	}
	_channels.push_back(Channel(chName));
	int chId = _channels.rbegin()->getId();
	_channels[chId].setOp(i, true);
	_channels[chId].setTopic("general");
	serverLog("channel created: ", _channels.rbegin()->getName());
	return (chId);
}

/**
 * @brief Handles the IRC JOIN command to join a channel.
 * 
 * Validates the command, checks channel conditions, and if successful,
 * adds the client to the channel and notifies all relevant parties.
 * 
 * @param i    The file descriptor index of the client sending the join request.
 * @param args The raw arguments: "<channel> [key]".
 * 
 * @note Sends ERR_BADCHANNELKEY (475) if the provided key is incorrect.
 * @note Sends ERR_CHANNELISFULL (471) if the channel has reached its user limit.
 * @note Sends ERR_INVITEONLYCHAN (473) if the channel is invite-only.
 * @note Sends ERR_USERONCHANNEL (443) if the user is already on the channel.
 * @note On success, broadcasts the join to the channel and sends RPL_TOPIC (332) to the client.
 */
void	Server::commandJoin(int i, std::string args)
{
	if (!isValidJoin(i, args))
		return ;

	std::string chName, key;
	setJoin(args, &chName, &key);
	if (!ischNameValid(chName))
		return (sendToClient(i, ERR_BADCHANMASK(_clients[i].getNick(), chName)));

	int chId = findOrCreateChannel(i, chName);
	if (key != _channels[chId].getChannelKey())
		return (sendToClient(i, ERR_BADCHANNELKEY(_clients[i].getNick(), chName)));
	if (_channels[chId].getClientsInChannel().size() >= _channels[chId].getLimit() && _channels[chId].getLimit() != 0)
		return (sendToClient(i, ERR_CHANNELISFULL(_clients[i].getNick(), chName)));
	if (_channels[chId].isInviteOnly())
		return (sendToClient(i, ERR_INVITEONLYCHAN(_clients[i].getNick(), chName)));

	if (isUserInChannel(i, chId))
		return (sendToClient(i, ERR_USERONCHANNEL(_clients[i].getNick(), chName)));

	_clients[i].setChannel(chId, chName);
	_channels[chId].addClient(i);
	std::string strToSend = _clients[i].getPrefix() + " JOIN " + chName;
	channelBroadcast(chId, strToSend);
	sendToClient(i, RPL_TOPIC(_clients[i].getNick(), chName, _channels[chId].getTopic()));

	std::string user_list;
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++)	{
		if (isUserInChannel(it->first, chId)) {
			if (_channels[chId].isOp(it->first))
				user_list += "@";
			user_list += _clients[it->first].getNick();
			user_list += " ";
		}
	}
	sendToClient(i, RPL_NAMREPLY(_clients[i].getNick(), _channels[chId].getName(), user_list));
}