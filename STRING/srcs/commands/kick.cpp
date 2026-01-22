#include "../includes/Server.hpp"
//todo PARSING, OUTPUT FOR KICKED CLIENT


/**
 * @brief Validates the KICK command arguments before execution.
 * 
 * Performs basic validation to ensure the client is registered and
 * that arguments were provided.
 * 
 * @param i    The file descriptor index of the client sending the kick.
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
 * @brief Parses the KICK command arguments into channel name and nickname to kick.
 * 
 * Splits the input line at the first space character to extract the
 * channel name and the nickname of the user to be kicked.
 * 
 * @param line        The raw arguments string: "<channel> <nickname>".
 * @param chName      Pointer to store the extracted channel name.
 * @param toKickName  Pointer to store the extracted nickname to kick.
 * 
 * @note Assumes the input line contains at least one space separator.
 */
void	setKick(std::string line, std::string *chName, std::string *toKickName)
{
	int pos = line.find(' ');
	*chName = line.substr(0, pos);
	*toKickName = line.substr(pos + 1);
}

//OPERATORS CAN KICK OTHER OPERATORS
/**
 * @brief Handles the IRC KICK command to remove a user from a channel.
 * 
 * Validates the command, checks permissions, and if successful,
 * removes the specified user from the channel and notifies all relevant parties.
 * 
 * @param i    The file descriptor index of the client sending the kick.
 * @param args The raw arguments: "<channel> <nickname>".
 * 
 * @note Sends ERR_NOTONCHANNEL (442) if the user to be kicked is not on the channel.
 * @note Sends ERR_NOPRIVILEGES (481) if the requesting client is not an operator.
 */
void	Server::commandKick(int i, std::string args)
{
	if (!isValidKick(i, args))
		return ;
	std::string chName, toKickName;
	setKick(args, &chName, &toKickName);
	
	int chId = getChannelId(chName);
	int toKickId = getClientId(toKickName);
	if (!isUserInChannel(toKickId, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));
	if (!_channels[chId].isOp(i))
		return (sendToClient(i, ERR_NOPRIVILEGES(_clients[i].getNick())));
	if (toKickId == i)
		return (sendToClient(i, " you cannot kick yourself FIX THIS STILL"));
	
	leaveChannel(toKickId, chId);
	//todo output for kicked client

	std::string strToSend = _clients[i].getPrefix() + " KICK " + chName + " " + toKickName;
	clientBroadcast(i, chId, strToSend);
}