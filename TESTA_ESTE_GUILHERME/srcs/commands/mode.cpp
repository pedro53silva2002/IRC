#include "../includes/Server.hpp"

bool	Server::isValidMode(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "MODE")), false);
	return (true);
}

void	Server::outputMode(int i, int chId, bool enable, char mode)
{
	char sign = (enable) ? '+' : '-';
	std::string strToSend = _clients[i].getPrefix() + " MODE " +  _channels[chId].getName() + " " + sign + mode;
	sendToClient(i, strToSend);
}

void	Server::modeInviteOnly(int i, int chId, bool inviteOnlyOrNot)
{
	_channels[chId].setInviteMode(inviteOnlyOrNot);
	outputMode(i, chId, inviteOnlyOrNot, 'i');
	if (inviteOnlyOrNot)
		serverLog(_channels[chId].getName(), " is now invite only");
	else
		serverLog(_channels[chId].getName(), " is now NOT invite only");
}
void	Server::modeTopicRestriction(int i, int chId, bool topicRestrict)
{
	_channels[chId].setTopicRestriction(topicRestrict);
	outputMode(i, chId, topicRestrict, 't');
	if (topicRestrict)
		serverLog(_channels[chId].getName(), " is now topic restricted");
	else
		serverLog(_channels[chId].getName(), " is now NOT topic restricted");
}
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


	if (opOrNot)
		serverLog(args, " is now op");
	else
		serverLog(args, " is now NOT op");
}
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