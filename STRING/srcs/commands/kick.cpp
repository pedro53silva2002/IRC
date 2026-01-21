#include "../includes/Server.hpp"
//todo PARSING, OUTPUT FOR KICKED CLIENT

/**
 * @brief Validates the KICK command arguments before execution.
 * 
 * Performs basic validation to ensure the client is registered and
 * that arguments were provided.
 * 
 * @param i    The file descriptor index of the client issuing the KICK.
 * @param args The raw arguments: "<channel> <nickname>".
 * 
 * @return true if basic validation passes.
 * @return false if validation fails (error sent to client).
 * 
 * @note Sends ERR_NOTREGISTERED (451) if client is not registered.
 * @note Sends ERR_NEEDMOREPARAMS (461) if no arguments provided.
 */
bool	Server::isValidKick(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "KICK")), false);
	return (true);
}

/**
 * @brief Parses KICK command arguments into channel and target components.
 * 
 * Extracts the channel name and the nickname of the user to kick
 * from the raw KICK command arguments string.
 * 
 * @param line       The raw arguments string in format "<channel> <nickname>".
 * @param chName     Output pointer to store the extracted channel name.
 * @param toKickName Output pointer to store the nickname of the user to kick.
 */
void	setKick(std::string line, std::string *chName, std::string *toKickName)
{
	int pos = line.find(' ');
	*chName = line.substr(0, pos);
	*toKickName = line.substr(pos + 1);
}

/**
 * @brief Handles the KICK command to remove a user from a channel.
 * 
 * Allows a channel operator to forcibly remove another user from the channel.
 * Operators can kick other operators. A user cannot kick themselves.
 * 
 * @param i    The file descriptor index of the client issuing the KICK.
 * @param args The raw arguments: "<channel> <nickname>".
 * 
 * @note Sends ERR_NOTONCHANNEL (442) if channel doesn't exist or target not in channel.
 * @note Sends ERR_NOPRIVILEGES (481) if client is not a channel operator.
 * @note Broadcasts KICK message to remaining channel members.
 * 
 * @see isValidKick() for basic validation (registration, params).
 * @see setKick() for parsing channel and target nickname.
 * @see leaveChannel() for removing the user from channel data structures.
 * 
 */
//OPERATORS CAN KICK OTHER OPERATORS
void	Server::commandKick(int i, std::string args)
{
	if (!isValidKick(i, args))
		return ;
	std::string chName, toKickName;
	setKick(args, &chName, &toKickName);
	
	int chId = getChannelId(chName);
	int toKickId = getClientId(toKickName);
	if (chId == -1 || !isUserInChannel(toKickId, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));
	if (!_channels[chId].isOp(i))
		return (sendToClient(i, ERR_NOPRIVILEGES(_clients[i].getNick())));
	if (toKickId == i)
		return (sendToClient(i, " you cannot kick yourself FIX THIS STILL"));
	
	leaveChannel(toKickId, chId);
	//todo output for kicked client

	std::string strToSend = _clients[i].getPrefix() + " KICK " + chName + " " + toKickName;
	clientBroadcast(i, chName, strToSend);//double check
}