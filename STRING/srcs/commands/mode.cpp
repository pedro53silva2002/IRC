#include "../includes/Server.hpp"

/**
 * @brief Validates the MODE command arguments before execution.
 * 
 * Performs basic validation to ensure the client is registered
 * and that arguments were provided.
 * 
 * @param i    The file descriptor index of the client sending the command.
 * @param args The raw arguments: "<channel> [modes]".
 * 
 * @return true if basic validation passes.
 * @return false if validation fails (error sent to client).
 * 
 * @note Sends ERR_NOTREGISTERED (451) if client is not registered.
 * @note Sends ERR_NEEDMOREPARAMS (461) if no arguments provided.
 */
bool	Server::isValidMode(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "MODE")), false);
	return (true);
}


//according to other irc, output is just for the client sending MODE
/**
 * @brief Sends a MODE change confirmation message to the client.
 * 
 * Constructs and sends a formatted MODE message indicating whether
 * a specific mode was enabled or disabled on a channel.
 * 
 * @param i      The file descriptor index of the client to notify.
 * @param chId   The ID of the channel where the mode was changed.
 * @param enable true if the mode was enabled, false if disabled.
 * @param mode   The mode character that was changed (e.g., 'i', 't', 'k', 'l').
 * 
 * @note The message format is: "<prefix> MODE <channel> <+/->mode".
 */
void	Server::outputMode(int i, int chId, bool enable, char mode)
{
	char sign = (enable) ? '+' : '-';
	std::string strToSend = _clients[i].getPrefix() + " MODE " +  _channels[chId].getName() + " " + sign + mode;
	sendToClient(i, strToSend);
}

/**
 * @brief Sets or unsets the invite-only mode (+i) on a channel.
 * 
 * Enables or disables the invite-only restriction on the specified channel,
 * notifies the client, and logs the change.
 * 
 * @param i              The file descriptor index of the client changing the mode.
 * @param chId           The ID of the channel to modify.
 * @param inviteOnlyOrNot true to enable invite-only mode, false to disable.
 * 
 * @note Sends MODE confirmation to the client via outputMode().
 * @note Logs the mode change to the server log.
 */
void	Server::modeInviteOnly(int i, int chId, bool inviteOnlyOrNot)
{
	_channels[chId].setInviteMode(inviteOnlyOrNot);
	outputMode(i, chId, inviteOnlyOrNot, 'i');
	if (inviteOnlyOrNot)
		serverLog(_channels[chId].getName(), " is now invite only");
	else
		serverLog(_channels[chId].getName(), " is now NOT invite only");
}

/**
 * @brief Sets or unsets the topic restriction mode (+t) on a channel.
 * 
 * Enables or disables the topic restriction on the specified channel.
 * When enabled, only channel operators can change the topic.
 * 
 * @param i             The file descriptor index of the client changing the mode.
 * @param chId          The ID of the channel to modify.
 * @param topicRestrict true to enable topic restriction, false to disable.
 * 
 * @note Sends MODE confirmation to the client via outputMode().
 * @note Logs the mode change to the server log.
 */
void	Server::modeTopicRestriction(int i, int chId, bool topicRestrict)
{
	_channels[chId].setTopicRestriction(topicRestrict);
	outputMode(i, chId, topicRestrict, 't');
	if (topicRestrict)
		serverLog(_channels[chId].getName(), " is now topic restricted");
	else
		serverLog(_channels[chId].getName(), " is now NOT topic restricted");
}

/**
 * @brief Sets or unsets the channel key mode (+k) on a channel.
 * 
 * Enables or disables the key (password) requirement for joining
 * the specified channel.
 * 
 * @param i      The file descriptor index of the client changing the mode.
 * @param chId   The ID of the channel to modify.
 * @param key    The channel key to set (ignored if setKey is false).
 * @param setKey true to set the channel key, false to remove it.
 * 
 * @note When disabling, the key is set to an empty string.
 * @note Sends MODE confirmation to the client via outputMode().
 * @note Logs the mode change to the server log.
 */
void	Server::modeKey(int i, int chId, std::string key, bool setKey)
{
	if (!setKey)
		_channels[chId].setChannelKey("");
	else
		_channels[chId].setChannelKey(key);
	outputMode(i, chId, setKey, 'k');
	if (setKey)
		serverLog(_channels[chId].getName(), " is now key entry only");
	else
		serverLog(_channels[chId].getName(), " is now NOT key entry only");
}

/**
 * @brief Sets or unsets the operator mode (+o) on a channel for a user.
 * 
 * Grants or revokes operator status for a specified user on a channel,
 * notifies all channel members, and logs the change.
 * 
 * @param i       The file descriptor index of the client changing the mode.
 * @param chId    The ID of the channel to modify.
 * @param args    The nickname of the user to op or de-op.
 * @param opOrNot true to grant operator status, false to revoke.
 * 
 * @note Sends error if the client tries to op themselves.
 * @note Sends error if the target user is not in the channel.
 * @note Broadcasts the mode change to all channel members.
 * @note Logs the mode change to the server log.
 */
void	Server::modeOp(int i, int chId, std::string args, bool opOrNot)
{
	if (_clients[i].getNick() == args)
		return (sendToClient(i, " FIX THIS OUTPUT, you cannot op yourself"));
	int toOpId = getClientId(args);
	if (!isUserInChannel(toOpId, chId))
		return (sendToClient(i, ERR_USERNOTINCHANNEL(_clients[i].getNick(), args, _channels[chId].getName())));

	_channels[chId].setOp(_clients[toOpId].getId(), opOrNot);//might replace for toOpId
	char sign = (opOrNot) ? '+' : '-';
	std::string strToSend = _clients[i].getPrefix() + " MODE " +  _channels[chId].getName() + " " + sign + "o " + _clients[toOpId].getNick();
	channelBroadcast(chId, strToSend);


	if (opOrNot)
		serverLog(args, " is now op");
	else
		serverLog(args, " is now NOT op");
}

/**
 * @brief Sets or removes the user limit mode (+l) on a channel.
 * 
 * Sets a maximum number of users allowed in the channel, or removes
 * the limit if an empty string is provided.
 * 
 * @param i        The file descriptor index of the client changing the mode.
 * @param chId     The ID of the channel to modify.
 * @param limitStr The user limit as a string, or empty to remove the limit.
 * 
 * @note If limitStr is empty, the limit is set to 0 (no limit).
 * @note Sends an error message if limitStr is not a valid number.
 * @note Sends MODE confirmation to the client via outputMode().
 * @note Logs the mode change to the server log.
 */
void	Server::modeLim(int i, int chId, std::string limitStr)
{
	int limit;
	if (limitStr.empty())
		limit = 0;
	else if (!isNum(limitStr))
		return sendToClient(i, limitStr + " is not a number");
	else 
		limit = atoi(limitStr.c_str());
	_channels[chId].setLimit(limit);
	outputMode(i, chId, limit, 'l');
	if (limit == 0)
		serverLog(_channels[chId].getName(), "now has NO limit");
	else
		serverLog(_channels[chId].getName(), "now has limit of " + limitStr);
}

/**
 * @brief Executes a single MODE operation on a channel.
 * 
 * Validates permissions and dispatches the mode change to the appropriate
 * handler based on the mode character.
 * 
 * @param i      The file descriptor index of the client executing the mode.
 * @param chName The name of the target channel.
 * @param opr    The mode operator string (e.g., "+i", "-t", "+o").
 * @param args   Additional arguments for modes that require them (e.g., nickname for +o).
 * 
 * @note Sends ERR_NOTONCHANNEL (442) if the client is not in the channel.
 * @note Sends ERR_NOPRIVILEGES (481) if the client is not a channel operator.
 * @note Sends ERR_UMODEUNKNOWNFLAG if the mode character is not recognized.
 * @note Supported modes: i (invite-only), t (topic restriction), k (key), o (operator), l (limit).
 */
void Server::executeCommandMode(int i, std::string chName, std::string opr, std::string args)
{
	int chId = getChannelId(chName);
	if (!isUserInChannel(i, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));
	if (!_channels[chId].isOp(i))
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
			sendToClient(i, ERR_UMODEWUNKNOWNFLAG + opr);
			break ;
	}
}

/**
 * @brief Handles the IRC MODE command for channels.
 * 
 * Parses the MODE command arguments, validates them, and executes
 * the requested mode changes on the specified channel.
 * 
 * @param i    The file descriptor index of the client sending the command.
 * @param line The raw arguments: "<channel> [modes]".
 * 
 * @note If no modes are provided, sends the current channel modes to the client.
 * @note Supports multiple mode changes in a single command.
 */
void	Server::commandMode(int i, std::string line)
{
	if (!isValidMode(i, line))
		return ;

	int pos = line.find(' ');
	std::string channelTarget = line.substr(0, pos);
	int chId = getChannelId(channelTarget);
	if (!isUserInChannel(i, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), channelTarget)));
	if (line == channelTarget)
		return (sendToClient(i, RPL_CHANNELMODEIS(_clients[i].getNick(), channelTarget)));
	
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