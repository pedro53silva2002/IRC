#include "../includes/Server.hpp"

/**
 * @brief Validates the PRIVMSG command arguments before execution.
 * 
 * Performs basic validation to ensure the client is registered
 * and that arguments were provided, including both target and message.
 * 
 * @param i    The file descriptor index of the client sending the message.
 * @param args The raw arguments: "<target> <message>".
 * 
 * @return true if basic validation passes.
 * @return false if validation fails (error sent to client).
 * 
 * @note Sends ERR_NOTREGISTERED (451) if client is not registered.
 * @note Sends ERR_NEEDMOREPARAMS (461) if no arguments provided.
 */
bool	Server::isValidPrivmsg(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "PRIVMSG")), false);
	
	size_t pos = args.find(' ');
	if (pos == std::string::npos || args.substr(pos + 1).empty())
		return (false);
	return (true);
}

/**
 * @brief Parses the PRIVMSG command arguments into target and message.
 * 
 * Splits the input arguments at the first space character to extract
 * the target (channel or user) and the message content.
 * 
 * @param args     The raw arguments string: "<target> <message>".
 * @param channel  Pointer to store the extracted target (channel or user).
 * @param message  Pointer to store the extracted message content.
 * 
 * @note If the message starts with ':', it is stripped from the stored message.
 */
void	setPrivmsg(std::string args, std::string *channel, std::string *message)
{
	size_t pos = args.find(' ');
	*channel = args.substr(0, pos);
	std::string rest = args.substr(pos + 1);
	if (rest[0] == ':')
		*message = rest.substr(1);
	else
		*message = rest.substr(0, rest.find(' '));
}

/**
 * @brief Handles the IRC PRIVMSG command to send a message to a channel.
 * 
 * Validates the command, checks channel membership, and if successful,
 * broadcasts the message to all clients in the specified channel.
 * 
 * @param i    The file descriptor index of the client sending the message.
 * @param args The raw arguments: "<target> <message>".
 * 
 * @note Sends ERR_NOTONCHANNEL (442) if the sender is not on the target channel.
 * @note Broadcasts the message in the format: "<prefix> PRIVMSG <channel> :<message>".
 */
void	Server::commandPrivmsg(int i, std::string args)
{
	if (!isValidPrivmsg(i, args))
		return ;
	
	std::string chName, message;
	setPrivmsg(args, &chName, &message);

	int chId = getChannelId(chName);
	if (!isUserInChannel(i, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));
	
	std::string toSend = _clients[i].getPrefix() + " PRIVMSG " + chName + " :" + message;
	clientBroadcast(i, chId, toSend);
}
