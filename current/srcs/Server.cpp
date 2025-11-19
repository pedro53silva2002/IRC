#include "../includes/Server.hpp"

//What happens if a client leaves a channel or disconnects? does the channel disappear? or does it give op to some other person?
//also, when a client that was op leaves his channel, and joins another, need to take op from him

//each time a command uses getBuf and receives a std::string, should parse it before running the rest of the function, to get like names or parameteres or whatever needed

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
	int take = 0;
	for (std::vector<Channel>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); ++channelIt)
	{
		if (name.substr(0, name.size()) == channelIt->getName())
			return (channelIt->getId());//Found an existing channel
	}
	if (_channels.size() == 1)
		take = 2;
	Channel temp(name.substr(0, name.size() - take));
	_channels.push_back(temp);
	_clients[i].setOp(true);
	std::cout << _channels.rbegin()->getName() << " has been created" << std::endl;
	return (_channels.rbegin()->getId());
}
void	Server::commandJoin(int i, std::string name)
{
	int channelId = findOrCreateChannel(i, name);
	_clients[i].setChannelId(channelId + 1);
	_clients[i].setchannelName( _channels[_clients[i].getChannelId()].getName());
	std::cout << "Client " << _clients[i].getNick() << 
				" joined channel " << _clients[i].getChannelName() << std::endl;

	std::string strToSend = "JOIN " + _channels[_clients[i].getChannelId()].getName();//check if this is whats supposed to be said
	sendToClientsInChannel(i, strToSend);

	std::string welcomeMessage =  "Welcome to the channel: " + _clients[i].getChannelName() + ", today's MOTD: temp motd!";//check if this is whats supposed to be said
	sendToClient(i, welcomeMessage);//THIS ONE IS NOT NEEDED, BUT ITS AN AGKNOWLEDGEMENT THAT CLIENT HAS JOINED
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
	/* std::string userToInvite (name);
	std::string channelToGet(name);
	Client *invitedClient;
	size_t pos = 0;
	//std::cout << "NAME BEFORE: " << channelToGet << std::endl;
	for (int i = 0; i < 3; ++i)
		pos = channelToGet.find(' ', pos + 1);
	channelToGet = channelToGet.substr(pos + 1, channelToGet.find(' ', pos + 2) - pos - 1);
	for (int i = 0; i < 2; ++i)
		pos = userToInvite.find(' ', pos + 1);
	userToInvite = userToInvite.substr(pos + 1, userToInvite.find(' ', pos + 2) - pos - 1); */
	/* std::cout << "NAME AFTER: " << userToInvite << std::endl;
	std::cout << "CHANNEL AFTER: " << channelToGet << std::endl; */
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
	/* for (std::vector<std::string>::iterator insideIt = channelsInside.begin(); insideIt != channelsInside.end(); ++insideIt)
	{
		if (channelToGet == *insideIt)
		{
			std::cout << "FOUND CHANNEL: " << *insideIt << std::endl;
			return (1);
		}
	} */
	//std::cout << "FOUND CHANNEL: " << std::endl;
	return (1);
}

void	Server::commandInvite(int i, std::string name)   ///////STILL DOING THIS DONT TOUCH
{
	std::string userToInvite (name);
	std::string channelToGet(name);
	size_t pos = 0;
	//std::cout << "NAME BEFORE: " << channelToGet << std::endl;
	for (int j = 0; j < 1; ++j)
		pos = channelToGet.find(' ', pos + 1);
	channelToGet = channelToGet.substr(pos + 1, channelToGet.find(' ', pos + 2) - pos - 1);
	for (int j = 0; j < 1; ++j)
		pos = userToInvite.find(' ', pos + 1);
	userToInvite = userToInvite.substr(pos + 1, userToInvite.find(' ', pos + 2) - pos - 1);
	int UserToInviteId;

	if (_clients[i].getChannelId() == -1) {
		std::cout << _clients[i].getNick() << " cannot invite users without being in any channel." << std::endl;
		sendToClient(_clients[i].getId(), "you cannot users without being in any channel");//!check the actual output
		return ;
	}
	if (findChannel(_clients[i], _channels, name, userToInvite, channelToGet))
		std::cout << "INVITED: " << name.substr(0, name.find(' ', 0))<< " by " << _clients[i].getNick() << std::endl;
	for (size_t i = 1; i < _clients.size(); i++)
	{
		if (userToInvite == _clients[i].getNick())
			UserToInviteId = i;
	}
	/* else
		std::cout << "Didnt found the channel to invite " << std::endl; */
	commandJoin(UserToInviteId, channelToGet);
	/* for (size_t i = 0; i < _channels.size(); i++)
	{
		std::cout << "WHERES THE CHANNEL NAMES: " << _channels[i].getName() << std::endl;
	} */
	//findChannel(_clients[i], _channels, name);
	/* for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		std::cout << "LOOKING FOR: " << name << " is it " << it->getNick() << std::endl;
		if (name == it->getNick()) {
			std::cout << "GOT IN" << std::endl;
			for (std::vector<Channel>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); ++channelIt)
			{
				if (it->getChannelName() != "" && channelIt->getName() == it->getChannelName())
				{
					std::cout << _clients[i].getNick() << " cannot invite users without being in the channel." << std::endl;
					sendToClient(_clients[i].getId(), "you cannot users without being in the channel");//!check the actual output
					return ;
				}
				std::cout << "CHECKING CHANNEL: " << channelIt->getName() << "my channel: " << it->getChannelName() << std::endl;
			}
		}
	} */
	/* for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (toKick == it->getNick() && it->getChannelId() == _clients[i].getChannelId()) {
			std::cout << _clients[i].getNick() << " kicked " << it->getNick() << " from " 
						<< _clients[i].getChannelName() << std::endl;
			it->setChannelId(-1);
			sendToClient(it->getId() + 1, "you have been kicked");//!check the actual output
			//!also why is this a +1 again? i thought i had solved this, check it when you want
		}
	}
	int channelId = findOrCreateChannel(i, name);
	_clients[i].setChannelId(channelId + 1);
	_clients[i].setchannelName( _channels[_clients[i].getChannelId()].getName());
	std::cout << "Client " << _clients[i].getNick() << 
				" joined channel " << _clients[i].getChannelName() << std::endl;

	std::string strToSend = "JOIN " + _channels[_clients[i].getChannelId()].getName();//check if this is whats supposed to be said
	sendToClientsInChannel(i, strToSend);

	std::string welcomeMessage =  "Welcome to the channel: " + _clients[i].getChannelName() + ", today's MOTD: temp motd!";//check if this is whats supposed to be said
	sendToClient(i, welcomeMessage);//THIS ONE IS NOT NEEDED, BUT ITS AN AGKNOWLEDGEMENT THAT CLIENT HAS JOINED */
}

void	Server::processCommand(int i)
{
	// debugMessage(i);
	// sendToClient(i, _clients[i].getBuf());

	//*Closing server
	if (strncmp(_clients[i].getBuf(), "exit ", 4) == 0)
		exitServer();
	//*Disconnects client
	else if (strncmp(_clients[i].getBuf(), "QUIT ", 4) == 0)
		return (commandQuit(i, "hardcoded quit"));
	
	//*Registering client
	else if (!_clients[i].isRegistered()) {
		registration(i);
		return ;
	}



	//*START OF CHANNEL LOGIC
	if (strncmp(_clients[i].getBuf(), "JOIN ", 5) == 0)
		commandJoin(i, _clients[i].getBuf() + 5);
	else if (strncmp(_clients[i].getBuf(), "KICK ", 5) == 0)
		commandKick(i, _clients[i].getBuf() + 5);
	else if (strncmp(_clients[i].getBuf(), "INVITE ", 7) == 0)
		commandInvite(i, _clients[i].getBuf() + 7); ///////STILL DOING THIS DONT TOUCH
	else
		sendToClientsInChannel(i, _clients[i].getBuf());

}


bool	Server::handleClientPoll(int i)
{
	char buf[512];
	_clients[i]._bytesRecv = myRecv(_pfds[i].fd, buf, sizeof(buf), 0);
	if (_clients[i]._bytesRecv == 0) {
		commandQuit(i, "");
		return (false);
	}
	_clients[i].setBuf(buf);

	processCommand(i);
	return (true);
}

void	Server::testClients()
{
	if (_clients.size() == 2) {
		_clients[1].setAuthenticated(true);
		_clients[1].setRegistered(true);
		_clients[1].setNick("First");
		_clients[1].setUsername("First");
		_clients[1].setRealname("First");
		// _clients[1].setChannelId(1);
		// Channel temp("FirstChannel");
		// _channels.push_back(temp);
		welcomeClient(1);
	}
	else if (_clients.size() == 3) {
		_clients[2].setAuthenticated(true);
		_clients[2].setRegistered(true);
		_clients[2].setNick("Second");
		_clients[2].setUsername("Second");
		_clients[2].setRealname("Second");
		// _clients[2].setChannelId(1);
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
	else if (_clients.size() == 5) {
		_clients[4].setAuthenticated(true);
		_clients[4].setRegistered(true);
		_clients[4].setNick("Fourth");
		_clients[4].setUsername("Fourth");
		_clients[4].setRealname("Fourth");
		// _clients[4].setChannelId(1);
		// Channel temp("FirstChannel");
		// _channels.push_back(temp);
		welcomeClient(4);
	}
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
