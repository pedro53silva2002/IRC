#include "../includes/Server.hpp"

/**
 * @brief Validates the PART command arguments before execution.
 * 
 * Performs basic validation to ensure the client is registered and
 * that arguments were provided.
 * 
 * @param i    The file descriptor index of the client sending the part.
 * @param args The raw arguments: "<channel> [reason]".
 * 
 * @return true if basic validation passes.
 * @return false if validation fails (error sent to client).
 * 
 * @note Sends ERR_NOTREGISTERED (451) if client is not registered.
 * @note Sends ERR_NEEDMOREPARAMS (461) if no arguments provided.
 */
bool	Server::isValidPart(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "PART")), false);
	return (true);
}

/**
 * @brief Removes a client from a channel.
 * 
 * Removes the channel from the client's channel list and removes the client
 * from the channel's member list. If the channel becomes empty, it is deleted.
 * 
 * @param i    The file descriptor index of the client leaving the channel.
 * @param chId The ID of the channel to leave.
 * 
 * @note If the channel has no remaining clients after removal, the channel is deleted.
 * @note Logs channel deletion to the server log when applicable.
 */
void	Server::leaveChannel(int i, int chId)
{
	_clients[i].getChannels().erase(chId);
	_channels[chId].removeClient(i);
	if (_channels[chId].getClientsInChannel().empty()) {
		serverLog(_channels[chId].getName(), "is empty, deleting");
		_channels.erase(_channels.begin() + chId);
		_channels[chId].decrementId();
	}
}

/**
 * @brief Parses the PART command arguments into channel name and optional reason.
 * 
 * Splits the input arguments at the first space character to extract
 * the channel name and, if present, the part reason.
 * 
 * @param args    The raw arguments string: "<channel> [reason]".
 * @param chName  Pointer to store the extracted channel name.
 * @param reason  Pointer to store the extracted reason (unchanged if no reason provided).
 * 
 * @note If no space is found, the entire args string is treated as the channel name.
 */
void	setPart(std::string args, std::string *chName, std::string *reason)
{
	size_t pos = args.find(' ');
	*chName = args.substr(0, pos);
	*reason = args.substr(pos + 1);
}

/**
 * @brief Handles the IRC PART command to leave a channel.
 * 
 * Validates the command, checks membership, and if successful,
 * removes the client from the channel and notifies all relevant parties.
 * 
 * @param i    The file descriptor index of the client sending the part.
 * @param args The raw arguments: "<channel> [reason]".
 * 
 * @note Sends ERR_NOTONCHANNEL (442) if the user is not on the channel.
 */
void	Server::commandPart(int i, std::string args)
{
	if (!isValidPart(i, args))
		return ;
	std::string chName, reason;
	setPart(args, &chName, &reason);
	int chId = getChannelId(chName);
	if (!isUserInChannel(i, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));

	leaveChannel(i, chId);
	std::string strToSend = _clients[i].getPrefix() + " PART " + chName;
	clientBroadcast(i, chId, strToSend);
	strToSend = _clients[i].getPrefix() + " PART " + chName;
	sendToClient(i, strToSend);
}


/**
 * @brief Handles the IRC QUIT command to disconnect a client.
 * 
 * Notifies all clients of the disconnection, removes the client
 * from all channels, closes the socket, and erases the client record.
 * 
 * @param i    The file descriptor index of the client sending the quit.
 * @param args The raw arguments: "[reason]".
 * 
 * @note Broadcasts the QUIT message to all clients.
 * @note Logs the disconnection to the server log.
 */
void	Server::commandQuit(int i, std::string args)
{
	serverLog(_clients[i].getNick(), "has disconnected");
	std::string strToSend = _clients[i].getPrefix() + " QUIT :Quit: " + args;
	serverBroadcast(strToSend);

	std::map<int, std::string> &chans = _clients[i].getChannels();
	for (std::map<int, std::string>::iterator it = chans.begin();  it != chans.end();) {
		int chId = it->first;
		++it;
		leaveChannel(i, chId);
	}

	close(_clients[i].getSocket());
	_clients.erase(i);
}