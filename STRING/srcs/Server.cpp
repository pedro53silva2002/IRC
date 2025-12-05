#include "../includes/Server.hpp"

//What happens if a client leaves a channel or disconnects? does the channel disappear? or does it give op to some other person?
//also, when a client that was op leaves his channel, and joins another, need to take op from him

//*CONSTRUCTORS
Server::Server(char *port, char *pass) {
	_name = "MyIRC";
	_port = atoi(port);
	_pass = pass;

	_socket = mySocket(AF_INET, SOCK_STREAM, 0);

	int opt = 1;
	setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(_port);
	inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr);//check "0.0.0.0"

	myBind(_socket, (sockaddr*)&server_addr, sizeof(server_addr));

	myListen(_socket, SOMAXCONN);

	std::cout << GREEN("Server open in port: ") << _port << std::endl;

	_srvPfd.fd = _socket;
	_srvPfd.events = POLLIN;
	_srvPfd.revents = 0;

	_clients.push_back(Client());//This is so that we dont have to work with _clients[i - 1]
	_channels.push_back(Channel());//This is so that we dont have to work with _channel[i - 1]
}

//*Accepting client
int		Server::acceptClient()
{
	int			tempSocket;
	sockaddr_in	clientAddr;
	socklen_t	clientSize = sizeof(clientAddr);
	char		host[NI_MAXHOST];
	
	//myAccept
	tempSocket = accept(_socket, (sockaddr*)&clientAddr, &clientSize);
	if (tempSocket == -1)
		throw (std::runtime_error("Problem with client connecting"));
	memset(host, 0, NI_MAXHOST);//useless?
	
	inet_ntop(AF_INET, &clientAddr.sin_addr, host, NI_MAXHOST);
	std::cout << host << " manually connected on " << ntohs(clientAddr.sin_port) << std::endl;
	return (tempSocket);
}

//*Set pollfds for loop
void	Server::setPfds()
{
	_pfds.clear();
	_pfds.push_back(_srvPfd);
	for (std::vector<Client>::iterator it = _clients.begin() + 1; it != _clients.end(); it++)
		_pfds.push_back(it->getPfd());
}

//*Disconnect client when client exits
void	Server::commandQuit(int i, std::string str)
{
	serverLog(_clients[i].getNick(), "has disconnected");
	//calling QUIT asks for reason, ctrl+c doesnt need reason
	sendToClient(i, "QUIT :" + str);
	close (_pfds[i].fd);
	_clients.erase(_clients.begin() + i);
}



int		Server::findOrCreateChannel(int i, std::string name)
{
	std::string channelTarget = name.substr(0, name.find(' ', 0));
	std::string channelKey;
	if (name.find(' ', 0) != std::string::npos)
		channelKey = name.substr(name.find(' ', 0) + 1);
	std::cout << "Channel: " << channelTarget << "\tKEY: " << channelKey << std::endl;
	for (std::vector<Channel>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); ++channelIt)
	{
		if (channelTarget == channelIt->getName())//change to use subster of the getName
		{	
			if (channelIt->getChannelKey() == "" || channelIt->getChannelKey() == channelKey)
				return (channelIt->getId());//Found an existing channel
			else
				return -1; //wrong key
		}
	}
	Channel temp(channelTarget);
	_channels.push_back(temp);
	_clients[i].setOp(true);
	std::cout << _channels.rbegin()->getName() << " has been created" << std::endl;
	return (_channels.rbegin()->getId());
}

void	Server::commandJoin(int i, std::string name)//parse so that only #channel is allowed
{
	int channelId = findOrCreateChannel(i, name);
	if (channelId == -1)
	{
		std::cout << "Wrong key provided for channel " << name << std::endl;
		sendToClient(_clients[i].getId(), "cannot join channel " + name + ": wrong key provided");
		return ;
	}
	if (_channels[channelId+1].getNbrClients() >= _channels[channelId+1].getMaxClients() && _channels[channelId+1].getMaxClients() != 0)
	{
		std::cout << "Channel " << _channels[channelId].getName() << " is full. Cannot join." << std::endl;
		sendToClient(_clients[i].getId(), "cannot join channel " + _channels[channelId].getName() + ": channel is full");
		return ;
	}
	else if (_channels[channelId].getInviteMode())
	{
		std::cout << "Channel " << _channels[channelId].getName() << " is invite-only. Cannot join." << std::endl;
		sendToClient(_clients[i].getId(), "cannot join channel " + _channels[channelId].getName() + ": channel is invite-only");
		return ;
	}
	_clients[i].setChannelId(channelId + 1);
	_clients[i].setchannelName( _channels[_clients[i].getChannelId()].getName());
	std::cout << "Client " << _clients[i].getNick() << 
				" joined channel " << _clients[i].getChannelName() << std::endl;
	_channels[_clients[i].getChannelId()].incrementNbrClients();
	/* std::cout << "Channel " << _channels[_clients[i].getChannelId()].getName() << 
				" now has " << _channels[_clients[i].getChannelId()].getNbrClients() << " users." << std::endl; */
	//OTHER MEMBERS OF CHANNEL KNOWING CLIENT JOINED
	// std::string strToSend = "JOIN " + _clients[i].getChannelName();
	// sendToClientsInChannel(i, strToSend);


	std::string joined = "JOIN :" + _clients[i].getChannelName();
	std::cout << _clients[i].getPrefix() << " :" << joined << std::endl;//THIS IS JUST BROKEN FUCK THIS
	sendToClient(_clients[i].getId(), _clients[i].getPrefix(), joined);//changed from i to getId()



	// std::string welcomeMessage =  "Welcome to the channel: " + _clients[i].getChannelName() + ", today's MOTD: temp motd!";//check if this is whats supposed to be said
	// sendToClient(i, welcomeMessage);
}



void	Server::commandKick(int i, std::string toKick)
{
	if (!_clients[i].getOp()) {
		std::cout << _clients[i].getNick() << " tried to Kick without being op" << std::endl;
		return ;
	}
	toKick = toKick.substr(0, toKick.size());
	if (toKick == _clients[i].getNick()) {
		std::cout << _clients[i].getNick() << " cannot kick themselves" << std::endl;
		sendToClient(_clients[i].getId(), "you cannot kick yourself from channel");//!check the actual output
		return ;
	}
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (toKick == it->getNick() && it->getChannelId() == _clients[i].getChannelId()) {
			std::cout << _clients[i].getNick() << " kicked " << it->getNick() << " from " 
						<< _clients[i].getChannelName() << std::endl;
			it->setChannelId(-1);
			sendToClient(it->getId() + 1, "you have been kicked");//!check the actual output
			//!also why is this a +1 again? i thought i had solved this, check it when you want
		}
	}
}

template<typename T>
bool hasInChannels(std::vector<T> channels, std::string name)
{
	for (typename std::vector<T>::iterator channelIt = channels.begin(); channelIt != channels.end(); ++channelIt)
	{
		if (name == channelIt->getName())
			return (true);
	}
	return (false);
}

bool hasInUserChannels(Client client, std::string name)
{
	std::vector<std::string> channelsInside;
	std::string line(client.getChannelName());
	//std::cout << "CLIENT CHANNEL NAMES: " << line << std::endl;
	size_t pos = 0;
	if (line == "")
		return (0);
	else if (line.find(';', pos + 1) == std::string::npos)
	{
		channelsInside.push_back(line);
	}
	else 
	{
		channelsInside.push_back(line.substr(0, line.find(';', pos)));
		while (line.find(';', pos + 1) != std::string::npos)
		{
			pos = line.find(';', pos + 1);
			//std::cout << "POS: " << pos << std::endl;
			channelsInside.push_back(line.substr(pos + 1, line.find(';', pos + 1) - pos - 1));
		}
		//std::cout << "LAST LINE?: " <<  line.find(';', pos) << std::endl;
	}
	for (std::vector<std::string>::iterator insideIt = channelsInside.begin(); insideIt != channelsInside.end(); ++insideIt)
	{
		//std::cout << "MINE: " << name << "\tCHECKING CHANNEL: " << *insideIt << "\nResult: " << strcmp(name.c_str(), (*insideIt).c_str()) << std::endl;
		if (name == *insideIt)
		{
			//std::cout << "FOUND CHANNEL: " << *insideIt << std::endl;
			return (true);
		}
	}
	return (false);
}

Client* Server::foundInUsers(std::string name)
{
	for (size_t i = 1; i < _clients.size(); i++)
	{
		if (name == _clients[i].getNick())
			return (&_clients[i]);
	}
	return (0);
}

int Server::findChannel(Client client, std::vector<Channel> channels,std::string name, std::string userToInvite, std::string channelToGet)
{
	Client *invitedClient;
	invitedClient = foundInUsers(userToInvite);
	if (!invitedClient)
	{
		std::cout <<"User to invite was not found in the list of users" << std::endl;
		return (0);
	}
	else if (!hasInChannels(channels, channelToGet))
	{
		std::cout <<"This channel was not found int the list of channels" << std::endl;
		return (0);
	}
	else if (!hasInUserChannels(client, channelToGet))
	{
		std::cout <<"This client is not in the channel" << std::endl;
		return (0);
	}
	else if (hasInUserChannels(*invitedClient, channelToGet))
	{
		std::cout <<"This invited client is already in the channel" << std::endl;
		return (0);
	}
	return (1);
}

void	Server::commandInvite(int i, std::string name)
{
	std::string userToInvite = name.substr(0, name.find(' ', 0));
	std::string channelToGet = name.substr(name.find(' ', 0) + 1);
	int index;
	size_t pos = 0;
	int UserToInviteId;

	if (_clients[i].getChannelId() == -1) {
		std::cout << _clients[i].getNick() << " cannot invite users without being in any channel." << std::endl;
		sendToClient(_clients[i].getId(), "you cannot users without being in any channel");//!check the actual output
		return ;
	}
	if (findChannel(_clients[i], _channels, name, userToInvite, channelToGet))
		std::cout << "INVITED: " << userToInvite << " by " << _clients[i].getNick() << std::endl;
	for (size_t i = 1; i < _clients.size(); i++)
	{
		if (userToInvite == _clients[i].getNick())
			UserToInviteId = i;
	}
	for (std::vector<Channel>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); ++channelIt)
	{
		if (channelToGet == channelIt->getName())//change to use subster of the getName
		{
			index = channelIt - _channels.begin();
			break ;
		}
	}
	/* else
		std::cout << "Didnt found the channel to invite " << std::endl; */
	//std::cout << "USER TO INVITE: " << userToInvite << "(" << _clients[UserToInviteId].getNick() << ")\t" << " CHANNEL TO GET: " << channelToGet << std::endl;
	//std::cout << "CHANNEL TO INVITE: " << index << std::endl;
	if (_channels[index].getChannelKey() != "")
		commandJoin(UserToInviteId, (channelToGet + " " + _channels[index].getChannelKey()));
	else
		commandJoin(UserToInviteId, (channelToGet));
	//commandJoin(UserToInviteId, (channelToGet + " " + _channels[index].getChannelKey()));
}

void Server::executeCommandMode(int i, std::string channelTarget, std::string opr, std::string user)
{
	if (opr == "+o")
	{
		if (isdigit(user[0]))
		{
			std::cout << "Invalid parameter: " << user << std::endl;
			return ;
		}
		//std::cout << _clients[i].getNick() << " is trying to give op to " << user << " in channel " << channelTarget << std::endl;
		Client *clientToOp = foundInUsers(user);
		if (!_clients[i].getOp())
			std::cout << _clients[i].getNick() << " tried to give op without being op" << std::endl;
		else if (strcmp(user.c_str(), _clients[i].getNick().c_str()) == 0)
			std::cout << _clients[i].getNick() << " tried to give himself ops -_-" << std::endl;
		else if (!hasInUserChannels(_clients[i], channelTarget))
			std::cout << _clients[i].getNick() << " is not in the channel to op someone in " << channelTarget << std::endl;
		else if (!clientToOp)
			std::cout << "Client to op not found" << std::endl;
		else if (!hasInUserChannels(*clientToOp, channelTarget))
			std::cout << clientToOp->getNick() << " is not in the cshannel to be opped in " << channelTarget << std::endl;
		else
		{
			clientToOp->setOp(true);
			std::cout << clientToOp->getNick() << " has been given op by " << _clients[i].getNick() << std::endl;
			sendToClient(clientToOp->getId(), "you have been given op in channel " + channelTarget + " by " + _clients[i].getNick());
		}
	}
	else if (opr == "-o")
	{
		Client *clientToOp = foundInUsers(user);
		if (!_clients[i].getOp())
			std::cout << _clients[i].getNick() << " tried to give op without being op" << std::endl;
		else if (strcmp(user.c_str(), _clients[i].getNick().c_str()) == 0)
			std::cout << _clients[i].getNick() << " tried to give himself ops -_-" << std::endl;
		else if (!hasInUserChannels(_clients[i], channelTarget))
			std::cout << _clients[i].getNick() << " is not in the channel to op someone in " << channelTarget << std::endl;
		else if (!clientToOp)
			std::cout << "Client to op not found" << std::endl;
		else if (!hasInUserChannels(*clientToOp, channelTarget))
			std::cout << clientToOp->getNick() << " is not in the cshannel to be opped in " << channelTarget << std::endl;
		else
		{
			clientToOp->setOp(false);
			std::cout << clientToOp->getNick() << " has been given op by " << _clients[i].getNick() << std::endl;
			sendToClient(clientToOp->getId(), "you have been given op in channel " + channelTarget + " by " + _clients[i].getNick());
		}
	}
	else if (opr == "+l")
	{
		for (size_t i = 0; i < user.size(); i++)
		{
			if (!isdigit(user[i]))
			{
				std::cout << "Invalid parameter: " << user << std::endl;
				return ;
			}
		}
		
		//std::cout << _clients[i].getNick() << " is trying to set limit in channel " << channelTarget << " to " << user << std::endl;
		int limit = atoi(user.c_str());
		if (!_clients[i].getOp())
			std::cout << _clients[i].getNick() << " tried to set limit without being op" << std::endl;
		else if (!hasInUserChannels(_clients[i], channelTarget))
			std::cout << _clients[i].getNick() << " is not in the channel to set limit in " << channelTarget << std::endl;
		else
		{
			for (std::vector<Channel>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); ++channelIt)
			{
				if (channelTarget == channelIt->getName())
				{
					if (channelIt->getNbrClients() > limit)
					{
						std::cout << "Cannot set limit to " << limit << " because there are already " << channelIt->getNbrClients() << " users in the channel" << std::endl;
						sendToClient(i, "cannot set limit to " + user + " because there are already smaller than the number of users in the channel");
						return ;
					}
					std::cout << "You have " << channelIt->getNbrClients() << " users out of max " << channelIt->getMaxClients() << std::endl;
					channelIt->setMaxClients(limit);
					std::cout << "Channel " << channelIt->getName() << " limit has been set to " << channelIt->getMaxClients() << " by " << _clients[i].getNick() << std::endl;
					sendToClient(i, "you have set the channel " + channelTarget + " limit to " + user);
				}
			}
		}
	}
	else if (opr == "-l")
	{
		/* if (user != "")
		{
			std::cout << "Cannot have extra parameter " << user << " for -l mode" << std::endl;
			return ;
		} */
		if (!_clients[i].getOp())
			std::cout << _clients[i].getNick() << " tried to take out user limit without being op" << std::endl;
		else if (!hasInUserChannels(_clients[i], channelTarget))
			std::cout << _clients[i].getNick() << " is not in the channel to take out user limit in " << channelTarget << std::endl;
		else
		{
			for (std::vector<Channel>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); ++channelIt)
			{
				if (channelTarget == channelIt->getName())
				{
					channelIt->setMaxClients(0);
					std::cout << "Channel " << channelIt->getName() << " limit has been removed by " << _clients[i].getNick() << std::endl;
				}
			}
		}
	}
	else if (opr == "+i")
	{
		/* if (user != "")
		{
			std::cout << "Cannot have extra parameter " << user << " for +i mode" << std::endl;
			return ;
		} */
		if (!_clients[i].getOp())
			std::cout << _clients[i].getNick() << " tried to enable invite only without being op" << std::endl;
		else if (!hasInUserChannels(_clients[i], channelTarget))
			std::cout << _clients[i].getNick() << " is not in the channel to enable invite only in " << channelTarget << std::endl;
		else
		{
			for (std::vector<Channel>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); ++channelIt)
			{
				std::cout << "MINE: " << channelTarget << "\tCHECKING CHANNEL: " << channelIt->getName() << "\nResult: " << strcmp(channelTarget.c_str(), channelIt->getName().c_str()) << std::endl;
				if (channelTarget == channelIt->getName())
				{
					channelIt->setInviteMode(1);
					std::cout << "INVITE MODE SET TO " << channelIt->getInviteMode() << std::endl;
					std::cout << "Channel " << channelIt->getName() << " invite-only mode has been enabled by " << _clients[i].getNick() << std::endl;
				}
			}
		}
	}
	else if (opr == "-i")
	{
		/* if (user != "")
		{
			std::cout << "Cannot have extra parameter " << user << " for -i mode" << std::endl;
			return ;
		} */
		if (!_clients[i].getOp())
			std::cout << _clients[i].getNick() << " tried to disable invite only without being op" << std::endl;
		else if (!hasInUserChannels(_clients[i], channelTarget))
			std::cout << _clients[i].getNick() << " is not in the channel to disable invite only in " << channelTarget << std::endl;
		else
		{
			for (std::vector<Channel>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); ++channelIt)
			{
				if (channelTarget == channelIt->getName())
				{
					channelIt->setInviteMode(0);
					std::cout << "Channel " << channelIt->getName() << " invite-only mode has been disabled by " << _clients[i].getNick() << std::endl;
				}
			}
		}
	}
	else if (opr == "+k")
	{
		/* if (user == "")
		{
			std::cout << "Need to give me the pass name." << std::endl;
			return ;
		} */
		if (!_clients[i].getOp())
			std::cout << _clients[i].getNick() << " tried to give a key without being op" << std::endl;
		else if (!hasInUserChannels(_clients[i], channelTarget))
			std::cout << _clients[i].getNick() << " is not in the channel to give a key in " << channelTarget << std::endl;
		else
		{
			for (std::vector<Channel>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); ++channelIt)
			{
				if (channelTarget == channelIt->getName())
				{
					channelIt->setChannelKey(user);
					std::cout << "Channel " << channelIt->getName() << " got a key cause of " << _clients[i].getNick() << std::endl;
				}
			}
		}
		//std::cout << "Channel key mode +k not implemented yet" << std::endl;
	}
	else if (opr == "-k")
	{
		/* if (user == "")
		{
			std::cout << "Need to give me the pass name." << std::endl;
			return ;
		} */
		if (!_clients[i].getOp())
			std::cout << _clients[i].getNick() << " tried to take a key being op" << std::endl;
		else if (!hasInUserChannels(_clients[i], channelTarget))
			std::cout << _clients[i].getNick() << " is not in the channel to take a key in " << channelTarget << std::endl;
		else
		{
			for (std::vector<Channel>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); ++channelIt)
			{
				if (channelTarget == channelIt->getName())
				{
					if (channelIt->getChannelKey() != user)
					{
						std::cout << "Cannot remove channel key, wrong key provided." << std::endl;
						sendToClient(i, "cannot remove channel key, wrong key provided");
						return ;
					}
					channelIt->setChannelKey("");
					std::cout << "Channel " << channelIt->getName() << " doesn't have a key cause of " << _clients[i].getNick() << std::endl;
				}
			}
		}
		//std::cout << "Channel key mode -k not implemented yet" << std::endl;
	}
	else if (opr == "+t")
	{
		if (!_clients[i].getOp())
			std::cout << _clients[i].getNick() << " tried to enable topic restriction without being op" << std::endl;
		else if (!hasInUserChannels(_clients[i], channelTarget))
			std::cout << _clients[i].getNick() << " is not in the channel to enable topic restriction in " << channelTarget << std::endl;
		else
		{
			for (std::vector<Channel>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); ++channelIt)
			{
				if (channelTarget == channelIt->getName())
				{
					channelIt->setTopicSet(true);
					std::cout << "Channel " << channelIt->getName() << " topic restriction has been enabled by " << _clients[i].getNick() << std::endl;
				}
			}
		}
	}
	else if (opr == "-t")
	{
		if (!_clients[i].getOp())
			std::cout << _clients[i].getNick() << " tried to disabled topic restriction without being op" << std::endl;
		else if (!hasInUserChannels(_clients[i], channelTarget))
			std::cout << _clients[i].getNick() << " is not in the channel to disabled topic restriction in " << channelTarget << std::endl;
		else
		{
			for (std::vector<Channel>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); ++channelIt)
			{
				if (channelTarget == channelIt->getName())
				{
					channelIt->setTopicSet(false);
					std::cout << "Channel " << channelIt->getName() << " topic restriction has been disabled by " << _clients[i].getNick() << std::endl;
				}
			}
		}
	}
	else
	{
		std::cout << "Unknown mode operation: " << opr << std::endl;
		sendToClient(i, "unknown mode operation: " + opr);
	}
}

void	Server::commandMode(int i, std::string line)
{
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
			/* if (line.substr(nextPos, line.find(' ', nextPos) - nextPos)[0] != '+' && line.substr(nextPos, line.find(' ', nextPos) - nextPos)[0] != '-')
				break ; */
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
	/* if (line.find(' ', pos + 1) == std::string::npos)
		opr = line.substr(pos + 1, line.size() - (pos + 1));
	else
	{
		opr = line.substr(pos + 1, line.find(' ', pos + 1) - (pos + 1));
		user = line.substr(line.find(' ', pos + 1) + 1, line.size() - line.find(' ', pos + 1));
	} */
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
				/* std::cout << "OPR PART: " << singleOpr <<  std::endl;
				std::cout << "USER PART: " << user.at(k) <<  std::endl; */
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
			/* std::cout << "OPR: " << *it <<  std::endl;
			std::cout << "USER: " << user.at(k) <<  std::endl; */
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


void	Server::commandTopic(int i, std::string line)
{
	size_t pos = line.find(' ', 0);
	std::string channelTarget = line.substr(0, pos);
	std::string topic;
	topic = line.substr(pos + 1);
	/* if (pos != std::string::npos)
		topic = line.substr(pos + 1);
	else
		topic = ""; */
	std::cout << "CHANNEL TARGET: " << channelTarget << "\nTOPIC: " << topic << std::endl;
	for (std::vector<Channel>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); ++channelIt)
	{
		if (channelTarget == channelIt->getName())
		{
			if (channelIt->getTopicSet() && !_clients[i].getOp())
			{
				std::cout << _clients[i].getNick() << " tried to set topic without being op in channel " << channelTarget << std::endl;
				sendToClient(i, "you cannot set topic in channel " + channelTarget + ": topic is restricted to ops");
				return ;
			}
			channelIt->setTopic(topic);
			std::cout << "Channel " << channelIt->getName() << " topic has been set to \"" << channelIt->getTopic() << "\" by " << _clients[i].getNick() << std::endl;
			sendToClient(i, "you have set the topic in channel " + channelTarget + " to: " + topic);
		}
	}
}



//compares can be called without inputing size

void	Server::processCommand(int i, std::string line)
{
	std::cout << YELLOW("Client ") << _clients[i].getNick()<< " said: " << line << std::endl;
	// sendToClient(i, line);

		
	//testing only
	if (line.compare(0, 4, "exit") == 0)
		exitServer();


	//*Registration commands

	if (line.compare(0, 5, "PASS ") == 0)//fix for without space, not enough parameters
		commandPass(i, line.substr(5));
	else if (line.compare(0, 5, "USER ") == 0)//fix for without space, not enough parameters
		commandUser(i, line.substr(5));
	else if (line.compare(0, 5, "NICK ") == 0)//fix for without space, not enough parameters
		commandNick(i, line.substr(5));

	
	else if (!_clients[i].isRegistered())//temporary, will be called by every function
		return serverLog(_clients[i].getNick(), "ISNT REGISTERED CANT TALK");
	
	
	else if (line.compare(0, 5, "JOIN ") == 0)
		commandJoin(i, line.substr(5));
	else if (line.compare(0, 5, "KICK ") == 0)
		commandKick(i, line.substr(5));
	else if (line.compare(0, 7, "INVITE ") == 0)
		commandInvite(i, line.substr(7));
	else if (line.compare(0, 5, "MODE ") == 0)
		commandMode(i, line.substr(5));
	else if (line.compare(0, 6, "TOPIC ") == 0)
		commandTopic(i, line.substr(6));
	else if (line.compare(0, 4, "QUIT") == 0)//move this down
		return (commandQuit(i, "hardcoded quit"));
	else
		sendToClientsInChannel(i, line);
	
	// else THIS WILL BE A PRIVMSG COMMAND
	// 	sendToClientsInChannel(i, line);

}

bool	Server::handleClientPoll(int i)
{
	char buf[512];
	memset(buf, 0, sizeof(buf));//new
	int bytesRecv = myRecv(_pfds[i].fd, buf, sizeof(buf), 0);
	if (bytesRecv == 0) {
		commandQuit(i, "");
		return (false);
	}

	/* buf[bytesRecv] = 0;
	
	//todo HERE I NEED TO DIVIDE OR SKIP \n AND \r
	//!stupid temporary fix for new line after input
	if (buf[bytesRecv - 1] == '\n' || buf[bytesRecv - 1] == '\r')
		buf[bytesRecv - 1] = 0; */
	for (size_t i = 1; i < bytesRecv - 1; i++)
	{
		if (buf[bytesRecv - i] == '\r' || buf[bytesRecv - i] == '\n')
		{
			buf[bytesRecv - i] = 0;
			break ;
		}
		else
			break ;
		i++;
	}

	

/* 
	ok big news. Apparently hexchat sends PASS USER and NICK all at the same time
	this means i need to find a way to skip each command?
	cause it it sends
	PASS <pass> USER <user> NICK <nick>
	all in the same poll, how do i split it?????
*/
	// if (line.empty() || line == "\n" || line == "\r" || line == "\r\n")//stupid fix
	// 	return true;
	std::string line(buf);
	line.erase(std::remove(line.begin(),line.end(), '\n'),line.end());
	line.erase(std::remove(line.begin(),line.end(), '\r'),line.end());

	processCommand(i, line);
	return (true);
}

/* char buf[512];
int bytes = recv(sock, buf, sizeof(buf)-1, 0);
if (bytes <= 0) return;

buf[bytes] = 0;
recv_buffer += buf;

// Process all complete lines
size_t pos;
while ((pos = recv_buffer.find("\r\n")) != std::string::npos) {
    std::string line = recv_buffer.substr(0, pos);
    recv_buffer.erase(0, pos + 2);

    if (!line.empty()) {
        handleCommand(line);
    }
} */

//todo to be put in client
std::string	Server::setPrefixTemp(int i)//!THIS NEEDS TO BE CALLED MANUALLY, FIX LATER, CAUSE NOW ITS ONLY IN HARDCODED
{
	//have to find a way to save host when not using hard coded clients
	return ":" + _clients[i].getNick() + "!" + _clients[i].getUsername() + "@" + _clients[i].getHost();
}

void	Server::testClients()
{
	if (_clients.size() == 2) {
		_clients[1].setAuthenticated(true);
		_clients[1].setRegistered(true);
		_clients[1].setHost(_name);
		_clients[1].setNick("First");
		_clients[1].setUsername("First");
		_clients[1].setRealname("First");
		_clients[1].setPrefix(setPrefixTemp(1));
		// _clients[1].setChannelId(1);
		// Channel temp("FirstChannel");
		// _channels.push_back(temp);
		welcomeClient(1);
	}
	else if (_clients.size() == 3) {
		_clients[2].setAuthenticated(true);
		_clients[2].setRegistered(true);
		_clients[2].setHost(_name);
		_clients[2].setNick("Second");
		_clients[2].setUsername("Second");
		_clients[2].setRealname("Second");
		_clients[2].setPrefix(setPrefixTemp(2));
		// _clients[2].setChannelId(1);
		// Channel temp("SecondChannel");
		// _channels.push_back(temp);
		welcomeClient(2);
	}
	// else if (_clients.size() == 4) {
	// 	_clients[3].setAuthenticated(true);
	// 	_clients[3].setRegistered(true);
	// 	_clients[3].setNick("Third");
	// 	_clients[3].setUsername("Third");
	// 	_clients[3].setRealname("Third");
	// 	// _clients[3].setChannelId(1);
	// 	// Channel temp("FirstChannel");
	// 	// _channels.push_back(temp);
	// 	welcomeClient(3);
	// }
}

void	Server::srvRun()
{
	while (1)
	{
		setPfds();
		myPoll(_pfds.data(), _pfds.size(), -1);
		
		if (_pfds[0].revents & POLLIN)//* Client Connecting
		{
			int temp = acceptClient();
			_clients.push_back(Client(temp));

			//HARDCODED CLIENTS AND CHANNELS
			testClients();
		}
	
		for (int i = 1; i < _pfds.size(); i++)//*loop through clients
		{
			if (_pfds[i].revents & POLLIN) {
				int ret = handleClientPoll(i);
				if (ret == false)//i dont like this
					continue ;
			}
		}
	}
	close(_socket);
}
