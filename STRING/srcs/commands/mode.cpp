#include "../includes/Server.hpp"
//OUTPUTS FOR CLIENT AND TO CHANNEL, PARSING, NUMERICS, DOUBLE CHECK
//todo parse op and lim?
//todo lim output
//todo removing key needs key?

/**
 * @brief Validates the MODE command arguments before execution.
 * 
 * Performs basic validation to ensure the client is registered and
 * that arguments were provided. Does not validate specific mode syntax.
 * 
 * @param i    The file descriptor index of the client in the _clients map.
 * @param args The raw MODE arguments: "<channel> <modes> [params...]".
 * 
 * @return true if basic validation passes.
 * @return false if validation fails (error sent to client).
 * 
 * @note Sends ERR_NOTREGISTERED (451) if client is not registered.
 * @note Sends ERR_NEEDMOREPARAMS (461) if no arguments provided.
 * 
 */
bool	Server::isValidMode(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "MODE")), false);
		// op needing a user to Op, 
		// key needing a key or nothing, 
		// limit needing a number or nothing
	return (true);
}

//according to other irc, output is just for the client sending MODE
/**
 * @brief Broadcasts a mode change notification to all channel members.
 * 
 * Constructs and sends the MODE message in IRC format to notify
 * all users in the channel about the mode change.
 * 
 * @param i      The file descriptor index of the client who made the change.
 * @param chId   The ID of the target channel.
 * @param enable true for '+' (enable mode), false for '-' (disable mode).
 * @param mode   The mode character (e.g., 'i', 't', 'k', 'o', 'l').
 * 
 * @note Output format: ":prefix MODE #channel +/-<mode>"
 * @note Used by mode handler functions for simple modes without arguments.
 */
void	Server::outputMode(int i, int chId, bool enable, char mode)
{
	char sign = (enable) ? '+' : '-';
	std::string strToSend = _clients[i].getPrefix() + " MODE " +  _channels[chId].getName() + " " + sign + mode;
	channelBroadcast(chId, strToSend);
}

/**
 * @brief Enables or disables invite-only mode on a channel (+i/-i mode).
 * 
 * When enabled, users can only join the channel if they have been invited.
 * When disabled, any user can join the channel freely.
 * 
 * @param i             The file descriptor index of the client in the _clients map.
 * @param chId          The ID of the target channel.
 * @param inviteOnlyOrNot true to enable invite-only (+i), false to disable (-i).
 * 
 * @note Broadcasts mode change to all channel members.
 */
void	Server::modeInviteOnly(int i, int chId, bool inviteOnlyOrNot)
{
	_channels[chId].setInviteMode(inviteOnlyOrNot);
	outputMode(i, chId, inviteOnlyOrNot, 'i');
}

/**
 * @brief Enables or disables topic restriction on a channel (+t/-t mode).
 * 
 * When enabled, only channel operators can change the topic.
 * When disabled, any channel member can change the topic.
 * 
 * @param i            The file descriptor index of the client in the _clients map.
 * @param chId         The ID of the target channel.
 * @param topicRestrict true to enable restriction (+t), false to disable (-t).
 * 
 * @note Broadcasts mode change to all channel members.
 */
void	Server::modeTopicRestriction(int i, int chId, bool topicRestrict)
{
	_channels[chId].setTopicRestriction(topicRestrict);
	outputMode(i, chId, topicRestrict, 't');
}

//+k needs to also send new password in the output
/**
 * @brief Sets or removes the channel key/password (+k/-k mode).
 * 
 * When enabled, users must provide the key to join the channel.
 * When disabled, the channel becomes open (no password required).
 * 
 * @param i      The file descriptor index of the client in the _clients map.
 * @param chId   The ID of the target channel.
 * @param key    The password to set (ignored if setKey is false).
 * @param setKey true to set the key (+k), false to remove it (-k).
 * 
 * @note Broadcasts mode change to all channel members.
 * 
 */
void	Server::modeKey(int i, int chId, std::string key, bool setKey)
{
	if (!setKey)
		_channels[chId].setChannelKey("");
	else
		_channels[chId].setChannelKey(key);
	outputMode(i, chId, setKey, 'k');
}

//if toOpId doesnt exist: ERR_NOSUCKNICK
/**
 * @brief Grants or revokes operator privileges for a user (+o/-o mode).
 * 
 * Changes the operator status of the target user in the specified channel.
 * A user cannot modify their own operator status.
 * 
 * @param i       The file descriptor index of the client caller in the _clients map.
 * @param chId    The ID of the target channel.
 * @param args    The nickname of the user to grant/revoke operator status.
 * @param opOrNot true to grant operator (+o), false to revoke (-o).
 * 
 * @note Fails if client tries to op/deop themselves.
 * @note Sends ERR_USERNOTINCHANNEL (441) if target user is not in the channel.
 * @note Broadcasts mode change to all channel members.
 * 
 */
void	Server::modeOp(int i, int chId, std::string args, bool opOrNot)
{
	if (_clients[i].getNick() == args)
		return (sendToClient(i, " FIX THIS OUTPUT, you cannot op yourself"));
	int toOpId = getClientId(args);
	if (!isUserInChannel(toOpId, chId))
		return (sendToClient(i, ERR_USERNOTINCHANNEL(_clients[i].getNick(), args, _channels[chId].getName())));

	_channels[chId].setOp(_clients[toOpId].getId(), opOrNot);
	char sign = (opOrNot) ? '+' : '-';
	std::string strToSend = _clients[i].getPrefix() + " MODE " +  _channels[chId].getName() + " " + sign + "o " + _clients[toOpId].getNick();
	channelBroadcast(chId, strToSend);
}

//+l needs to also send new limit in the output
/**
 * @brief Sets or removes the user limit on a channel (+l/-l mode).
 * 
 * Applies a maximum user limit to the channel. If limit is 0, the limit
 * is removed. Will not apply a limit lower than the current user count.
 * 
 * @param i        The file descriptor index of the client in the _clients map.
 * @param chId     The ID of the target channel.
 * @param limitStr The new limit as a string (or empty/"0" to remove limit).
 * 
 * @note Fails silently if the new limit is less than current member count.
 * @note Broadcasts mode change to all channel members.
 * 
 */
void	Server::modeLim(int i, int chId, std::string limitStr)
{
	int limit = atoi(limitStr.c_str());
	if (_channels[chId].getClientsInChannel().size() > limit && limit != 0)
		return serverLog(_clients[i].getNick(), "limit cannot be set to " + limitStr + ": too many people already");
	_channels[chId].setLimit(limit);
	serverLog(_clients[i].getNick(), _channels[chId].getName() + " CHANNEL LIMIT CHANGED");
	
	std::string strToSend = _clients[i].getPrefix() + " MODE " +  _channels[chId].getName() + " " + "WHAT DO I PUT HERE" + "l";
	channelBroadcast(chId, strToSend);
}

//BADCHANMASK before getChannelId?
//RPL_CHANNELMODEIS?
/**
 * @brief Executes a single mode change on a channel.
 * 
 * Validates operator privileges and dispatches to the appropriate mode
 * handler based on the mode character. Called by commandMode() after parsing.
 * 
 * @param i      The file descriptor index of the client in the _clients map.
 * @param chName The target channel name.
 * @param opr    The mode operation string (e.g., "+i", "-k", "+o").
 * @param args   Optional argument for modes that require it (key, nick, limit).
 * 
 * @note Sends ERR_NOTONCHANNEL (442) if client is not in the channel.
 * @note Sends ERR_NOPRIVILEGES (481) if client is not a channel operator.
 * @note Sends ERR_UMODEUNKNOWNFLAG for unrecognized mode characters.
 * 
 * @see modeInviteOnly() for +i/-i (invite-only)
 * @see modeTopicRestriction() for +t/-t (topic restriction)
 * @see modeKey() for +k/-k (channel key)
 * @see modeOp() for +o/-o (operator status)
 * @see modeLim() for +l/-l (user limit)
 */
void Server::executeCommandMode(int i, std::string chName, std::string opr, std::string args)
{
	int chId = getChannelId(chName);
	if (!isUserInChannel(i, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));
	if (!_channels[chId].isOp(_clients[i].getId()))
		return (sendToClient(i, ERR_NOPRIVILEGES(_clients[i].getNick())));

	char sign = opr[0];
	bool enable = (sign == '+');
	char mode = opr[1];
	switch (mode)
	{
		case 'i':
			modeInviteOnly(i, chId, enable);
			break ;
		case 't':
			modeTopicRestriction(i, chId, enable);
			break ;
		case 'k':
			modeKey(i, chId, args, enable);
			break ;
		case 'o':
			modeOp(i, chId, args, enable);
			break ;
		case 'l':
			modeLim(i, chId, args);
			break ;
		default:
		// ERR_UNKNOWNMODE instead?
			sendToClient(i, ERR_UMODEWUNKNOWNFLAG + opr);
			break ;
	}
}

//TODO HAVE A FUNCTION THAT PARSES THIS COMMAND
/**
 * @brief Handles the MODE command for setting channel modes.
 * 
 * Parses the MODE command line to extract the target channel, mode flags,
 * and optional arguments. Supports multiple mode changes in a single command
 * (e.g., "+it-k" or "+o user1 +o user2"). Each mode is then executed individually.
 * 
 * Supported modes:
 * - i: Invite-only channel
 * - t: Topic restriction (operators only)
 * - k: Channel key (password)
 * - o: Operator privilege
 * - l: User limit
 * 
 * @param i    The file descriptor index of the client in the _clients map.
 * @param line The raw MODE command arguments: "<channel> <modes> [args...]".
 * 
 * @note Compound modes like "+itk" are split into individual "+i", "+t", "+k" calls.
 * @see isValidMode() for initial validation (registration, params).
 * @see executeCommandMode() for executing each individual mode change.
 * 
 */
void	Server::commandMode(int i, std::string line)
{
	if (!isValidMode(i, line))
		return ;
	
	int pos = line.find(' ', 0);
	std::string channelTarget = line.substr(0, pos);
	std::vector<std::string> opr;
	std::vector<std::string> user;
	size_t nextPos = pos + 1;
	if (line.find(' ', nextPos) == std::string::npos)
	{
		opr.push_back(line.substr(nextPos, line.size() - nextPos));
		user.push_back("");
	}
	else
	{
		while (line.substr(nextPos, line.find(' ', nextPos) - nextPos)[0] == '+' || line.substr(nextPos, line.find(' ', nextPos) - nextPos)[0] == '-')
		{
			opr.push_back(line.substr(nextPos, line.find(' ', nextPos) - nextPos));
			nextPos = line.find(' ', nextPos) + 1;
		}
		while (line.find(' ', nextPos) != std::string::npos && nextPos < line.size())
		{
			user.push_back(line.substr(nextPos, line.find(' ', nextPos) - nextPos));
			nextPos = line.find(' ', nextPos) + 1;
		}
		if (nextPos)
			user.push_back(line.substr(nextPos, line.size() - nextPos));
	}
	int k = 0;
	for (std::vector<std::string>::iterator it = opr.begin(); it != opr.end(); ++it)
	{
		if ((*it).size() > 2)
		{
			for (size_t j = 1; j < (*it).size(); j++)
			{
				std::string singleOpr;
				singleOpr += (*it)[0];
				singleOpr += (*it)[j];
				executeCommandMode(i, channelTarget, singleOpr, user.at(k));
				if (k < user.size())
					k++;
			}
		}
		else
			executeCommandMode(i, channelTarget, *it, user.at(k));
		if (k < user.size())
			k++;
	}
}