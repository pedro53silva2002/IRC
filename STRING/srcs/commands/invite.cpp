#include "../includes/Server.hpp"

//ERR_USERONCHANNEL
//ERR_CHANNELISFULL, +l
//TODO PARSE

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
 * @brief Parses INVITE command arguments into nickname and channel components.
 * 
 * Extracts the invited user's nickname and the target channel name
 * from the raw INVITE command arguments string.
 * 
 * @param line        The raw arguments string in format "<nickname> <channel>".
 * @param invitedName Output pointer to store the invited user's nickname.
 * @param chName      Output pointer to store the channel name.
 */
void	setInvite(std::string line, std::string *invitedName, std::string *chName)
{
	int pos = line.find(' ');
	*invitedName = line.substr(0, pos);
	*chName = line.substr(pos + 1);
}
//CHANOPRIVSNEEDEDE if inviteOnly?
//RPL_INVITING to i
//INVITE message to invited user
/**
 * @brief Handles the INVITE command to invite a user to a channel.
 * 
 * Allows a channel member to invite another user to join the channel.
 * The invited user is automatically added to the channel upon invitation.
 * 
 * @param i    The file descriptor index of the client sending the invite.
 * @param args The raw arguments: "<nickname> <channel>".
 * 
 * @note Sends ERR_NOTONCHANNEL (442) if inviter is not in the channel.
 * @note Sends ERR_USERONCHANNEL (443) if invited user is already in the channel.
 * @note Broadcasts invite message to all channel members.
 * 
 * @see isValidInvite() for basic validation (registration, params).
 * @see setInvite() for parsing nickname and channel name.
 * 
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

	int invitedId = getClientId(invitedName);
	//check if this output is the same
	if (isUserInChannel(invitedId, chId))
		return (sendToClient(invitedId, ERR_USERONCHANNEL(_clients[invitedId].getNick(), chName)));

	//!check if key is needed in case its locked channel, prob not because user inviting the other is in channel
	_clients[invitedId].setChannel(chId, chName);
	_channels[chId].addClient(invitedId);
	std::string strToSend = _clients[i].getPrefix() + " INVITED " + invitedName + " TO THE CHANNEL, CHECK OUTPUT";
	channelBroadcast(chId, strToSend);
}
