#include "../includes/Server.hpp"
//todo OUTPUTS FOR CLIENT AND TO CHANNEL, PARSING, NUMERICS, DOUBLE CHECK

// sendToAllClientsInChannel needs to be called where its needed

//check numerics and sendtoallclients outputs
void	Server::modeInviteOnly(int i, std::string channelTarget, bool inviteOnlyOrNot)
{
	//todo check extra parameters and parsing

	for (std::vector<Channel>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); ++channelIt)
	{
		if (channelTarget == channelIt->getName()) {
			channelIt->setInviteMode(inviteOnlyOrNot);
			//!outputs
			serverLog(_clients[i].getNick(), channelTarget + " changed invite only restrictions");
		}
	}
}
void	Server::modeTopicRestriction(int i,std::string channelTarget, bool topicRestrict)
{
	//todo check extra parameters and parsing
		
	for (std::vector<Channel>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); ++channelIt)
	{
		if (channelTarget == channelIt->getName()) {
			channelIt->setTopicRestriction(topicRestrict);
			//!outputs
			serverLog(_clients[i].getNick(), channelTarget + "LOG IT TO SAY WHAT CHANGED");
			// serverLog(_clients[i].getNick(), channelTarget + " topic restrictions have been changed LOG IT TO SAY WHAT CHANGED");
			// clientBroadcast(i, "channel topic restrictions changed");//todo check output
		}
	}
}
void	Server::modeOp(int i, std::string channelTarget, std::string user, bool opOrNot)
{
	//todo check extra parameters and parsing
	Client *clientToOp = foundInUsers(user);
	if (!clientToOp)//todo THIS IS BAD
	{
		std::cout << "Client to op not found" << std::endl;
		return ;
	}

	if (_clients[i].getNick() == user)
		return (sendToClient(i, "you cannot op yourself -_-"));//todo check output

	if (!hasInUserChannels(*clientToOp, channelTarget))
		return (sendToClient(i, ERR_USERNOTINCHANNEL(_clients[i].getNick(), clientToOp->getNick(), channelTarget)));

	_channels[clientToOp->getChannelId()].setOp(clientToOp->getId(), opOrNot);
	//todo this isnt sendToClient, its sendToAllClientsInChannel
	//!outputs
	sendToClient(i, OPERATOR(_clients[i].getNick(), channelTarget, clientToOp->getNick()));
}
void	Server::modeLim(int i, std::string channelTarget, std::string limitStr)//shouldnt accept +l 0
{
	//todo check extra parameters and parsing
		
	int limit = atoi(limitStr.c_str());
	for (std::vector<Channel>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); ++channelIt)
	{
		if (channelTarget == channelIt->getName())
		{
			//probably should allow limit to be smaller than users in channel, but dont let new users join, like discord
			if (channelIt->getNbrClients() > limit && limit != 0)
			{
				//!outputs
				serverLog(_clients[i].getNick(), "limit cannot be set to " + limitStr + ": too many people already");
				return ;
			}
			channelIt->setLimit(limit);
			break ;
		}
	}
	//!outputs
	if (limit == 0)//change for -1 so default is simpler to understand
		serverLog(_clients[i].getNick(), "channel limit has been removed");
	//!outputs
	else
		serverLog(_clients[i].getNick(), " set " + channelTarget + " limit to " + limitStr);
}


//doing, coletes help
void	Server::modeKey(int i, std::string channelTarget, std::string key, bool setKey)//find a way to remove setKey
{
	//todo check extra parameters and parsing
	
	for (std::vector<Channel>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); ++channelIt)
	{
		if (channelTarget == channelIt->getName())
		{
			if (setKey == false) {
				channelIt->setChannelKey("");
				serverLog(_clients[i].getNick(), channelIt->getName() + " doesnt neet key anymore");
			}
			else {
				channelIt->setChannelKey(key);
				serverLog(_clients[i].getNick(), channelIt->getName() + " now has key" + key);
			}
		}
	}
	//std::cout << "Channel key mode k not implemented yet" << std::endl;
}


void Server::executeCommandMode(int i, std::string chName, std::string opr, std::string args)//user more like arguments
{
	//todo make sure its well parsed
	if (!hasInUserChannels(_clients[i], chName))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), chName)));//todo check output
	
	if (!_channels[_clients[i].getChannelId()].getOp(_clients[i].getId()))
		return (sendToClient(i, ERR_NOPRIVILEGES(_clients[i].getNick())));//save nick in var
	
	char sign = opr[0];
	bool enable = (sign == '+');
	char mode = opr[1];
	switch (mode)
	{
		case 'i':
			modeInviteOnly(i, chName, enable);
			break ;
		case 't':
			modeTopicRestriction(i, chName, enable);
			break ;
		case 'k':
			modeKey(i, chName, args, enable);
			break ;
		case 'o':
			modeOp(i, chName, args, enable);
			break ;
		case 'l':
			modeLim(i, chName, args);
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
