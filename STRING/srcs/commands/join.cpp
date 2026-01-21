#include "../includes/Server.hpp"

//todo PARSING AND KEY

//also &
/**
 * @brief Validates the JOIN command arguments before execution.
 * 
 * Performs basic validation to ensure the client is registered,
 * arguments were provided, and the channel name starts with '#'.
 * 
 * @param i    The file descriptor index of the client in the _clients map.
 * @param args The raw arguments: "<channel> [key]".
 * 
 * @return true if basic validation passes.
 * @return false if validation fails (error sent to client).
 * 
 * @note Sends ERR_NOTREGISTERED (451) if client is not registered.
 * @note Sends ERR_NEEDMOREPARAMS (461) if no arguments provided.
 * @note Sends ERR_NOSUCHCHANNEL (403) if channel doesn't start with '#'.
 * 
 */
bool	Server::isValidJoin(int i, std::string args)
{
	if (!_clients[i].isRegistered()) 
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "JOIN")), false);
	if (args[0] != '#')
		return (sendToClient(i, ERR_NOSUCHCHANNEL(args)), false);
	return (true);
}

/**
 * @brief Parses JOIN command arguments into channel name and key components.
 * 
 * Extracts the channel name and optional key from the raw JOIN
 * command arguments string.
 * 
 * @param args   The raw arguments string in format "<channel> [key]".
 * @param chName Output pointer to store the extracted channel name.
 * @param key    Output pointer to store the channel key (unchanged if no key provided).
 * 
 * @note The key parameter is only modified if a key is present in args.
 */
void	setJoin(std::string args, std::string *chName, std::string *key)
{
	int pos = args.find(' ');
	*chName = args.substr(0, pos);
	if (pos != std::string::npos)
		*key = args.substr(pos + 1);
}

/**
 * @brief Finds an existing channel or creates a new one.
 * 
 * Searches for a channel by name. If found, returns its ID.
 * If not found, creates a new channel and sets the requesting
 * client as the channel operator.
 * 
 * @param i      The file descriptor index of the client requesting the channel.
 * @param chName The name of the channel to find or create.
 * 
 * @return The ID of the existing or newly created channel.
 * 
 * @note When creating a new channel, the client becomes the operator.
 * @note Logs channel creation to the server log.
 */
int		Server::findOrCreateChannel(int i, std::string chName)
{
	for (int j = 0; j < _channels.size(); j++) {
		if (chName == _channels[j].getName())
			return (_channels[j].getId());
	}
	//setTopic to whatever
	_channels.push_back(Channel(chName));
	int chId = _channels.rbegin()->getId();
	_channels[chId].setOp(_clients[i].getId(), true);
	serverLog("channel created: ", _channels.rbegin()->getName());
	return (chId);
}

/**
 * @brief Handles the JOIN command to enter a channel.
 * 
 * Allows a client to join an existing channel or create a new one.
 * Validates channel restrictions (key, limit, invite-only) before joining.
 * The first user to join a new channel becomes the channel operator.
 * 
 * @param i    The file descriptor index of the client joining.
 * @param args The raw arguments: "<channel> [key]".
 * 
 * @note Sends ERR_BADCHANNELKEY (475) if the channel key is incorrect.
 * @note Sends ERR_CHANNELISFULL (471) if the channel user limit is reached.
 * @note Sends ERR_INVITEONLYCHAN (473) if the channel is invite-only.
 * @note Sends ERR_USERONCHANNEL (443) if the user is already in the channel.
 * @note Broadcasts JOIN message to all channel members.
 * 
 * @see isValidJoin() for basic validation (registration, params, # prefix).
 * @see setJoin() for parsing channel name and key.
 * @see findOrCreateChannel() for channel lookup/creation.
 * 
 */
void	Server::commandJoin(int i, std::string args)
{
	if (!isValidJoin(i, args))
		return ;

	std::string chName, key;
	setJoin(args, &chName, &key);
	
	int chId = findOrCreateChannel(i, chName);
	if (key != _channels[chId].getChannelKey())
	//PSSWDMISMATCH instead?
		return (sendToClient(i, ERR_BADCHANNELKEY(_clients[i].getNick(), chName)));
	if (_channels[chId].getClientsInChannel().size() >= _channels[chId].getLimit() && _channels[chId].getLimit() != 0)
		return (sendToClient(i, ERR_CHANNELISFULL(_clients[i].getNick(), chName)));
	if (_channels[chId].isInviteOnly())
		return (sendToClient(i, ERR_INVITEONLYCHAN(_clients[i].getNick(), chName)));

	if (isUserInChannel(i, chId))
		return (sendToClient(i, ERR_USERONCHANNEL(_clients[i].getNick(), chName)));

	_clients[i].setChannel(chId, chName);
	//todo  RPL_TOPIC2
	//todo  RPL_NAMREPLY (create a user list)
	/* 
	//!AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	//!AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	//!AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	//!AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	//!AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	//!AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	//!AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	//!AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	//!AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	//!AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	//!AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	//!AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	//!AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	//!AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	//!AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	//!AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAV
	*/
	_channels[chId].addClient(i);
	std::string strToSend = _clients[i].getPrefix() + " JOIN " + chName;
	channelBroadcast(chId, strToSend);

}