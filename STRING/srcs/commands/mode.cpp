#include "../includes/Server.hpp"
//OUTPUTS FOR CLIENT AND TO CHANNEL, PARSING, NUMERICS, DOUBLE CHECK
//todo parse op and lim?
//todo lim output
//todo removing key needs key?

/*
	todo parsing for args:
		op needing a user to Op, 
		key needing a key or nothing, 
		limit needing a number or nothing
*/

//*redone
void	Server::modeInviteOnly(int i, int chId, bool inviteOnlyOrNot)
{
	_channels[chId].setInviteMode(inviteOnlyOrNot);
	char sign = (inviteOnlyOrNot) ? '+' : '-';
	std::string strToSend = _clients[i].getPrefix() + " MODE " +  _channels[chId].getName() + " " + sign + "i";
	channelBroadcast(i, _channels[chId].getName(), strToSend);
}
void	Server::modeTopicRestriction(int i, int chId, bool topicRestrict)
{
	_channels[chId].setTopicRestriction(topicRestrict);
	char sign = (topicRestrict) ? '+' : '-';
	std::string strToSend = _clients[i].getPrefix() + " MODE " +  _channels[chId].getName() + " " + sign + "t";
	channelBroadcast(i, _channels[chId].getName(), strToSend);
}
void	Server::modeKey(int i, int chId, std::string key, bool setKey)
{
	if (!setKey)
		_channels[chId].setChannelKey("");
	else
		_channels[chId].setChannelKey(key);
	char sign = (setKey) ? '+' : '-';
	std::string strToSend = _clients[i].getPrefix() + " MODE " +  _channels[chId].getName() + " " + sign + "k";
	channelBroadcast(i, _channels[chId].getName(), strToSend);
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
	channelBroadcast(i, _channels[chId].getName(), strToSend);
}
void	Server::modeLim(int i, int chId, std::string limitStr)
{
	int limit = atoi(limitStr.c_str());
	if (_channels[chId].getNbrClients() > limit && limit != 0)
		return serverLog(_clients[i].getNick(), "limit cannot be set to " + limitStr + ": too many people already");
	_channels[chId].setLimit(limit);
	serverLog(_clients[i].getNick(), _channels[chId].getName() + " CHANNEL LIMIT CHANGED");
	
	std::string strToSend = _clients[i].getPrefix() + " MODE " +  _channels[chId].getName() + " " + "WHAT DO I PUT HERE" + "l";
	channelBroadcast(i, _channels[chId].getName(), strToSend);
}


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
			sendToClient(i, ERR_UMODEWUNKNOWNFLAG + opr);
			break ;
	}
}

void	Server::commandMode(int i, std::string line)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())));
		//TODO HAVE A FUNCTION THAT PARSES THIS COMMAND
	if (line.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "MODE")));
	
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
			// if (line.substr(nextPos, line.find(' ', nextPos) - nextPos)[0] != '+' && line.substr(nextPos, line.find(' ', nextPos) - nextPos)[0] != '-')
			// 	break ;
			opr.push_back(line.substr(nextPos, line.find(' ', nextPos) - nextPos));
			nextPos = line.find(' ', nextPos) + 1;
		}
		while (line.find(' ', nextPos) != std::string::npos && nextPos < line.size())
		{
			//std::cout << "FOUND EMPTY SPACE" << std::endl;
			user.push_back(line.substr(nextPos, line.find(' ', nextPos) - nextPos));
			nextPos = line.find(' ', nextPos) + 1;
		}
		//std::cout << "NEXTPOS: " << nextPos << std::endl;
		if (nextPos)
			user.push_back(line.substr(nextPos, line.size() - nextPos));
	}
	// if (line.find(' ', pos + 1) == std::string::npos)
	// 	opr = line.substr(pos + 1, line.size() - (pos + 1));
	// else
	// {
	// 	opr = line.substr(pos + 1, line.find(' ', pos + 1) - (pos + 1));
	// 	user = line.substr(line.find(' ', pos + 1) + 1, line.size() - line.find(' ', pos + 1));
	// }
	//std::cout << "CHANNEL TARGET: " << channelTarget << "\nOPR: " << opr << "\nUSER: " << user << std::endl;
	//std::cout << "Result: " << strcmp(opr.c_str(), "+k") << std::endl;
	//std::cout << "MODE command received from " << _clients[i].getNick() << " with params: " << line << std::endl;
	//std::cout << "CHANNEL TARGET: " << channelTarget << std::endl;
	//std::vector<std::string>::iterator userIt = user.begin();
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
				// std::cout << "OPR PART: " << singleOpr <<  std::endl;
				// std::cout << "USER PART: " << user.at(k) <<  std::endl;
				executeCommandMode(i, channelTarget, singleOpr, user.at(k));
				if (k < user.size())
				{
					k++;
				}
			}
		}
		else
		{
			executeCommandMode(i, channelTarget, *it, user.at(k));
			// std::cout << "OPR: " << *it <<  std::endl;
			// std::cout << "USER: " << user.at(k) <<  std::endl;
		}
		if (k < user.size())
		{
			k++;
		}
		//executeCommandMode(i, channelTarget, *it, user);
	}
	
	//std::cout << "USER: " << user << std::endl;
	
	//mode logic here
	//sendToClient(i, "MODE command received with params: " + line);
}
