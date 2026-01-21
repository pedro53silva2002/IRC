#include "../includes/Server.hpp"
//todo PARSING
//parse QUIT, quit can get a <reason>, //!FIX QUIT OUTPUT


/**
 * @brief Validates the PART command arguments before execution.
 * 
 * Checks that the client is registered and that a channel name
 * argument was provided.
 * 
 * @param i    The file descriptor index of the client in the _clients map.
 * @param args The channel name argument from the PART command.
 * 
 * @return true if the command is valid and can proceed.
 * @return false if validation fails (error sent to client).
 * 
 * @note Sends ERR_NOTREGISTERED (451) if client is not registered.
 * @note Sends ERR_NEEDMOREPARAMS (461) if no channel name is provided.
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
 * @brief Removes a client from a channel's data structures.
 * 
 * Performs the cleanup required when a client leaves a channel by
 * removing them from the channel's client list and removing the
 * channel from the client's channel map.
 * 
 * @param i    The file descriptor index of the client in the _clients map.
 * @param chId The ID of the channel to leave.
 * 
 */
void	Server::leaveChannel(int i, int chId)
{
	_channels[chId].removeClient(i);
	_clients[i].getChannels().erase(chId);
}

/**
 * @brief Handles the PART command for leaving a channel.
 * 
 * Validates the request, then removes the client from the specified channel
 * and broadcasts the departure to all remaining channel members.
 * 
 * @param i    The file descriptor index of the client in the _clients map.
 * @param args The channel name to leave.
 * 
 * @note Sends ERR_NOSUCHCHANNEL (403) if the channel does not exist.
 * @note Sends ERR_NOTONCHANNEL (442) if the client is not in the channel.
 */
void	Server::commandPart(int i, std::string args)
{
	if (!isValidPart(i, args))
		return ;
	int chId = getChannelId(args);
	if (chId == -1)
		return (sendToClient(i, ERR_NOSUCHCHANNEL(args)));
	if (!isUserInChannel(i, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), args)));

	std::string strToSend = _clients[i].getPrefix() + " PART " + args;
	channelBroadcast(chId, strToSend);
	leaveChannel(i, chId);
}

/**
 * @brief Handles the QUIT command for client disconnection.
 * 
 * Disconnects the client from the server by broadcasting a quit message,
 * removing them from all channels, closing their socket, and erasing
 * them from the clients map.
 * 
 * @param i    The file descriptor index of the client in the _clients map.
 * @param args Optional quit reason/message provided by the client.
 * 
 * @note Broadcasts quit message to all connected clients on the server.
 * @note Removes client from all channels they were a member of.
 * 
 */
void	Server::commandQuit(int i, std::string args)
{
	serverLog(_clients[i].getNick(), "has disconnected");
	std::string strToSend = _clients[i].getPrefix() + " QUIT :Quit: " + args;
	serverBroadcast(strToSend);

	for (std::map<int, std::string>::iterator it = _clients[i].getChannels().begin(); 
		it != _clients[i].getChannels().end(); it++)
			leaveChannel(i, it->first);

	close(i);
	_clients.erase(i);
}