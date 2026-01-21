#include "../includes/Server.hpp"
//todo DO PRIVATE MESSAGES

/*
	check this output: 
	PRIVMSG #CHANNEL :
*/

/**
 * @brief Validates the PRIVMSG command arguments before execution.
 * 
 * Checks that the client is registered and that the arguments contain
 * both a valid target and a non-empty message.
 * 
 * @param i    The file descriptor index of the client in the _clients map.
 * @param args The raw arguments string in format "<target> <message>".
 * 
 * @return true if the command is valid and can be executed.
 * @return false if validation fails (error sent to client).
 * 
 * @note Sends ERR_NOTREGISTERED (451) if client is not registered.
 * @note Sends ERR_NEEDMOREPARAMS (461) if args is empty.
 */
bool	Server::isValidPrivmsg(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "PRIVMSG")), false);
	
	int pos = args.find(' ');
	if (pos == std::string::npos || args.substr(pos + 1).empty())
		return (false);
	return (true);
}

/**
 * @brief Parses PRIVMSG arguments into channel/target and message components.
 * 
 * Extracts the target (channel or user) and message content from the raw
 * PRIVMSG arguments string. Handles both colon-prefixed messages (full text)
 * and non-prefixed messages (first word only).
 * 
 * @param args    The raw arguments string in format "<target> [:]<message>".
 * @param channel Output pointer to store the extracted target name.
 * @param message Output pointer to store the extracted message content.
 * 
 * @note If message starts with ':', the entire remaining string is used.
 * @note If message has no ':', only the first word is taken as the message.
 */
void	setPrivmsg(std::string args, std::string *channel, std::string *message)
{
	int pos = args.find(' ');
	*channel = args.substr(0, pos);
	std::string rest = args.substr(pos + 1);
	if (rest[0] == ':')
		*message = rest.substr(1);
	else
		*message = rest.substr(0, rest.find(' '));
}

/**
 * @brief Handles the PRIVMSG command for sending messages to channels.
 * 
 * Parses and validates the PRIVMSG command, then broadcasts the message
 * to all users in the target channel (except the sender).
 * 
 * @param i    The file descriptor index of the client in the _clients map.
 * @param args The raw arguments string containing "<target> :<message>".
 * 
 * @note Sends ERR_NOTONCHANNEL (442) if the sender is not in the target channel.
 * @see isValidPrivmsg() for validation (checks registration and params).
 * @see setPrivmsg() for parsing the channel name and message content.
 * 
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
