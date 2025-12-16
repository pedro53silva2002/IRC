#include "../includes/Server.hpp"

//What happens if a client leaves a channel or disconnects? does the channel disappear? or does it give op to some other person?
//also, when a client that was op leaves his channel, and joins another, need to take op from him

//check order of parsing, like isOp, isInChannel

//prefix needs to be sent as str before calling sendtoclientsinchannel
//calling commands without being registered, need to fix that

//replace every for loop that uses iterators to just run through the vector

//change getNick in sendtoclients to a var nick
//change args var name to line

//*HARDCODE CHANNELS WITH DIFFERENT MODES TO TEST EVERYTHING
//todo PART AND QUIT need to remove client from channel in channel-side object

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
//! NEEDS TO REMOVE CLIENT FROM CHANNELS
void	Server::commandQuit(int i, std::string str)
{
	serverLog(_clients[i].getNick(), "has disconnected");
	//calling QUIT asks for reason, ctrl+c doesnt need reason
	sendToClient(i, "QUIT :" + str);
	close (_pfds[i].fd);
	_clients.erase(_clients.begin() + i);
}


void testString(std::string line)
{
	std::cout << RED("[") << line << RED("]\n");
}


//*NEED TO DOUBLE CHECK KEY AND PARSING, HARDCODE A CHANNEL WITH A KEY IF NEEDED
void	setJoin(std::string args, std::string *chName, std::string *key)
{
	int pos = args.find(' ');
	*chName = args.substr(0, pos);
	if (pos != std::string::npos)
		*key = args.substr(pos + 1);
}

int		Server::findOrCreateChannel(int i, std::string name)
{
	std::string channelTarget = name.substr(0, name.find(' ', 0));


	for (int j = 0; j < _channels.size(); j++) {
		if (channelTarget == _channels[j].getName())
			return (_channels[j].getId());
	}
	_channels.push_back(Channel(channelTarget));
	_clients[i].setOp(true);
	std::cout << _channels.rbegin()->getName() << " has been created" << std::endl;
	return (_channels.rbegin()->getId());
}

void	Server::commandJoin(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())));
	//todo parse: find the #; find if there is a key, and test it
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "JOIN")));
	if (args[0] != '#')
		return (sendToClient(i, ERR_BADCHANMASK(_clients[i].getNick(), args)));

	std::string chName, key;
	setJoin(args, &chName, &key);

	
	int channelId = findOrCreateChannel(i, args);
	if (key != _channels[channelId].getChannelKey())
		return (sendToClient(i, ERR_BADCHANNELKEY(_clients[i].getNick(), chName)));
	if (_channels[channelId].getNbrClients() >= _channels[channelId].getLimit() && _channels[channelId].getLimit() != 0)
		return (sendToClient(i, ERR_CHANNELISFULL(_clients[i].getNick(), chName)));
	if (_channels[channelId].isInviteOnly())
		return (sendToClient(i, ERR_INVITEONLYCHAN(_clients[i].getNick(), chName)));


	_clients[i].setChannelId(channelId);
	_clients[i].setchannelName( _channels[_clients[i].getChannelId()].getName());//!does client really need the name?
	_channels[_clients[i].getChannelId()].incrementNbrClients();

	sendToClient(i, "You have joined channel " + chName);

	//outputs
//RPL_TOPIC
//RPL_NAMREPLAY
//send list of names
//RPL_JOIN for everyone
	//*OTHER MEMBERS OF CHANNEL KNOWING CLIENT JOINED
	// std::string strToSend = nick + " joined " + _clients[i].getChannelName();
	// sendToClientsInChannel(i, strToSend);
	//if (topic != empty)
	// sendToClient(i, RPL_TOPIC(nick, chName, "TEMP TOPIC"));

}

/* divide into string chName and string Key, if key isnt empty, check key

	int pos = args.find(' ');
	if (pos != std::string::npos)// "<channel> " or "<channel> <key>"
	{
		std::string key = args.substr(pos + 1);
		if (key.empty());
			//ignore
	}
*/

bool Server::hasInChannels(std::string name)
{
	for (int i = 0; i < _channels.size(); i++) {
		if (name == _channels[i].getName())
			return (true);
	}
	return (false);
}

void	Server::commandPart(int i, std::string name)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())));
	if (name.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "PART")));

	if (!hasInChannels(name))//remake this function
		return (sendToClient(i, ERR_NOSUCHCHANNEL(_clients[i].getNick(), name)));
	
	sendToClient(i, "You have left channel " + name);//!check the actual output
	std::string strToSend = _clients[i].getNick() + " left " + name;
	sendToClientsInChannel(i, strToSend);
	_clients[i].setChannelId(-1);
	_clients[i].setchannelName("");
}




//todo NEEDMOREPARAMS CAUSE YOU NEED "PRIVMSG <client> <channel>", check order
void	Server::commandKick(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())));
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "KICK")));

	
	if (!_clients[i].getOp())//todo put back
		return (sendToClient(i, ERR_NOPRIVILEGES(_clients[i].getNick())));
	

	std::string chName = args.substr(0, args.find(' '));//!PARSE
	std::string toKick = args.substr(args.find(' ') + 1);//!PARSE
	// std::cout << RED("channel name: ") << chName << RED(", toKick name: ") << toKick << std::endl;
	if (toKick == _clients[i].getNick())
		return (sendToClient(i, " you cannot kick yourself FIX THIS STILL"));//todo fix output

	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (toKick == it->getNick()) {
			//check toKick is in channel
			if (it->getChannelId() != _clients[i].getChannelId())
				return (sendToClient(i, ERR_USERNOTINCHANNEL(_clients[i].getNick(), toKick, chName)));
			sendToClientsInChannel(i, _clients[i].getNick() + " KICK " + chName + " " + toKick);//todo + prefix
			it->setChannelId(-1);
			it->setchannelName("");
		}
	}


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
	else if (!hasInChannels(channelToGet))
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

//ERR_USERONCHANNEL
//ERR_CHANNELISFULL, +l
void	Server::commandInvite(int i, std::string name)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())));
	//TODO HAVE A FUNCTION THAT PARSES THIS COMMAND
	if (name.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "INVITE")));

	
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


//*MODES

//have a function that returns the channelTarget, that way it gets simpler
// sendToAllClientsInChannel needs to be called where its needed

//todo done except outputs and parsing, and sendToAllClients all changes that happen
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
			// sendToClientsInChannel(i, "channel topic restrictions changed");//todo check output
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

	clientToOp->setOp(opOrNot);
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


void Server::executeCommandMode(int i, std::string channelTarget, std::string opr, std::string args)//user more like arguments
{
	//todo make sure its well parsed
	if (!hasInUserChannels(_clients[i], channelTarget))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), channelTarget)));//todo check output
	
	if (!_clients[i].getOp())
		return (sendToClient(i, ERR_NOPRIVILEGES(_clients[i].getNick())));//save nick in var
	
	char sign = opr[0];
	bool enable = (sign == '+');
	char mode = opr[1];
	switch (mode)
	{
		case 'i':
			modeInviteOnly(i, channelTarget, enable);
			break ;
		case 't':
			modeTopicRestriction(i, channelTarget, enable);
			break ;
		case 'k':
			modeKey(i, channelTarget, args, enable);
			break ;
		case 'o':
			modeOp(i, channelTarget, args, enable);
			break ;
		case 'l':
			modeLim(i, channelTarget, args);
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

//*MODES END

//todo broken if client has left the channel
void	Server::commandTopic(int i, std::string line)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())));
		//TODO HAVE A FUNCTION THAT PARSES THIS COMMAND
	if (line.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "TOPIC")));

	
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
			if (channelIt->isTopicRestricted() && !_clients[i].getOp())
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





//*mostly done
bool	Server::isValidPrivmsg(std::string line)
{
	int pos = line.find(' ');
	if (pos == std::string::npos || line.substr(pos + 1).empty())
		return (false);
	return (true);
}
void	setPrivmsg(std::string line, std::string *channel, std::string *message)
{
	int pos = line.find(' ');
	*channel = line.substr(0, pos);
	std::string rest = line.substr(pos + 1);
	if (rest[0] == ':')
		*message = rest.substr(1);
	else
		*message = rest.substr(0, rest.find(' '));
}
void	Server::commandPrivmsg(int i, std::string line)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())));
	if (line.empty() || !isValidPrivmsg(line))
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "PRIVMSG")));
	
	std::string channel, message;
	setPrivmsg(line, &channel, &message);
	if (_clients[i].getChannelId() == -1 || _clients[i].getChannelName() != channel)
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), channel)));
	std::string toSend = _clients[i].getPrefix() + " PRIVMSG " + channel + " :" + message;

	sendToClientsInChannel(i, toSend);
}


//todo i should skip all whitespaces
std::string parseLine(std::string line)
{
	int pos = line.find(' ');
	if (pos == std::string::npos)
		return ("");
	std::string arguments = line.substr(pos + 1);
	return (arguments);
}


void	Server::processCommand(int i, std::string line)
{
	if (line.compare(0, 11, "CAP LS 302") == 0)//todo figure out what to do
		return ;
	if (line.compare(0, 4, "QUIT") == 0)
		return commandQuit(i, "HARDCODED");
	else if (line.compare(0, 4, "exit") == 0)
		return exitServer();
	else if (line.compare(0, 4, "test") == 0)
		return sendToClientsInChannel(i, "THIS IS A TEST MESSAGE");



	
	typedef void (Server::*funcs)(int, std::string);
	std::string commands[] = {"PASS", "USER", "NICK", "JOIN", "PART", "PRIVMSG", "KICK", "INVITE", "MODE", "TOPIC", "QUIT"};

	funcs function[] = {&Server::commandPass, &Server::commandUser, &Server::commandNick, &Server::commandJoin, &Server::commandPart,
	&Server::commandPrivmsg, &Server::commandKick, &Server::commandInvite, &Server::commandMode, &Server::commandTopic, &Server::commandQuit};
	std::string temp = line.substr(0, line.find(' '));
	for (int j = 0; j < 11; j++) {
		if (commands[j] == temp) {
			(this->*function[j])(i, parseLine(line));
			return ;
		}
	}
	sendToClient(i, ERR_UNKNOWNCOMMAND(_clients[i].getNick(), line));
}

bool	Server::handleClientPoll(int i)
{
	char		buf[512];
	std::string	recv_buffer;

	memset(buf, 0, sizeof(buf));
	int bytesRecv = myRecv(_pfds[i].fd, buf, sizeof(buf), 0);
	if (bytesRecv == 0) {
		commandQuit(i, "");
		return (false);
	}
	buf[bytesRecv] = 0;
	recv_buffer += buf;

	size_t pos;
	while ((pos = recv_buffer.find("\r\n")) != std::string::npos) {
		std::string line = recv_buffer.substr(0, pos);
		recv_buffer.erase(0, pos + 2);

		if (!line.empty())
			processCommand(i, line);
	}
	return (true);
}

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
		// _clients[1].setchannelName("FIRST");
		// _channels.push_back(Channel("FIRST"));
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
		// _clients[2].setchannelName("FIRST");
		// Channel temp("SecondChannel");
		// _channels.push_back(temp);
		welcomeClient(2);
	}
	else if (_clients.size() == 4) {
		_clients[3].setAuthenticated(true);
		_clients[3].setRegistered(true);
		_clients[3].setNick("Third");
		_clients[3].setUsername("Third");
		_clients[3].setRealname("Third");
		// _clients[3].setChannelId(1);
		// Channel temp("FirstChannel");
		// _channels.push_back(temp);
			welcomeClient(3);
	}
}

void	Server::srvRun()
{
	while (1)
	{
		setPfds();
		myPoll(_pfds.data(), _pfds.size(), -1);
		
		if (_pfds[0].revents & POLLIN)//*Client Connecting
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
