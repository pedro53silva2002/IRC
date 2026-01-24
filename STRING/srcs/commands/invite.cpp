#include "../includes/Server.hpp"

/**
 * @brief Validates the INVITE command arguments before execution.
 * 
 * Performs basic validation to ensure the client is registered and
 * that arguments were provided.
 * 
 * @param i    The file descriptor index of the client sending the invite.
 * @param args The raw arguments: "<nickname> <channel>".
 * 
 * @return true if basic validation passes.
 * @return false if validation fails (error sent to client).
 * 
 * @note Sends ERR_NOTREGISTERED (451) if client is not registered.
 * @note Sends ERR_NEEDMOREPARAMS (461) if no arguments provided.
 */
bool	Server::isValidInvite(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "INVITE")), false);
	return (true);
}

/**
 * @brief Parses the INVITE command arguments into nickname and channel name.
 * 
 * Splits the input line at the first space character to extract the
 * invited user's nickname and the target channel name.
 * 
 * @param line        The raw arguments string: "<nickname> <channel>".
 * @param invitedName Pointer to store the extracted nickname.
 * @param chName      Pointer to store the extracted channel name.
 * 
 */
void	setInvite(std::string line, std::string *invitedName, std::string *chName)
{
	int pos = line.find(' ');
	*invitedName = line.substr(0, pos);
	*chName = line.substr(pos + 1);
}

/**
 * @brief Handles the IRC INVITE command to invite a user to a channel.
 * 
 * Validates the command, checks permissions, and if successful, adds
 * the invited user to the channel and notifies all relevant parties.
 * 
 * @param i    The file descriptor index of the client sending the invite.
 * @param args The raw arguments: "<nickname> <channel>".
 * 
 * @note Sends ERR_NOTONCHANNEL (442) if the inviter is not on the channel.
 * @note Sends ERR_CHANOPRIVSNEEDED (482) if channel is invite-only and inviter is not an operator.
 * @note Sends ERR_USERONCHANNEL (443) if the invited user is already on the channel.
 * @note On success, broadcasts the invite to the channel, sends RPL_TOPIC (332) to the invited user,
 *       and sends RPL_INVITING (341) to the inviter.
 */
void	Server::commandInvite(int i, std::string args)
{
	if (!isValidInvite(i, args))
		return ;
	
	std::string invitedName, chName;
	setInvite(args, &invitedName, &chName);

	int chId = getChannelId(chName);
	if (!isUserInChannel(i, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));
	if (!_channels[chId].isOp(i) && _channels[chId].isInviteOnly())
		return (sendToClient(i, ERR_CHANOPRIVSNEEDED(_clients[i].getNick(), chName)));

	int invitedId = getClientId(invitedName);
	if (isUserInChannel(invitedId, chId))
		return (sendToClient(invitedId, ERR_USERONCHANNEL(_clients[invitedId].getNick(), chName)));

	_clients[invitedId].setChannel(chId, chName);
	_channels[chId].addClient(invitedId);
	std::string strToSend = _clients[i].getPrefix() + " INVITE " + invitedName + " " + chName;
	channelBroadcast(chId, strToSend);
	sendToClient(invitedId, RPL_TOPIC(_clients[invitedId].getNick(), chName, _channels[chId].getTopic()));
	sendToClient(i, RPL_INVITING(invitedName, _clients[i].getNick(), chName));
}
