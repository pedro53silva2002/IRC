#include "../includes/Server.hpp"
//OUTPUTS FOR CLIENT AND TO CHANNEL, PARSING, NUMERICS, DOUBLE CHECK
//todo parse op and lim?
//todo lim output
//todo removing key needs key?

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
void	Server::outputMode(int i, int chId, bool enable, char mode)
{
	char sign = (enable) ? '+' : '-';
	std::string strToSend = _clients[i].getPrefix() + " MODE " +  _channels[chId].getName() + " " + sign + mode;
	channelBroadcast(chId, strToSend);
}

void	Server::modeInviteOnly(int i, int chId, bool inviteOnlyOrNot)
{
	_channels[chId].setInviteMode(inviteOnlyOrNot);
	outputMode(i, chId, inviteOnlyOrNot, 'i');
}
void	Server::modeTopicRestriction(int i, int chId, bool topicRestrict)
{
	_channels[chId].setTopicRestriction(topicRestrict);
	outputMode(i, chId, topicRestrict, 't');
}

//+k needs to also send new password in the output
void	Server::modeKey(int i, int chId, std::string key, bool setKey)
{
	if (!setKey)
		_channels[chId].setChannelKey("");
	else
		_channels[chId].setChannelKey(key);
	outputMode(i, chId, setKey, 'k');
}

//if toOpId doesnt exist: ERR_NOSUCKNICK
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