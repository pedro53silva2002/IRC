#include "../includes/Server.hpp"


/*
	COLETES os comandos precisam de ser parsed para:
		<Comando>				(sem mais nada, nem o espaço)
		<Comando> 				(com o espaço)
		<Comando> I				(um só caracter)
	
	especificamente, preciso também do commandJoin me guarde uma variavel chamada channelName, 
	que esteja já só com o nome que o client der input. (Exemplo: JOIN Channel1   ->  channelName = Channel1)

*/



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


//this is just testing
void	Server::exitServer()
{
	std::cout << "exiting server" << std::endl;
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
		close(it->getSocket());
	close(_socket);
	throw (0);
}


void	Server::sendToClient(int id, std::string sender, std::string str)
{
	std::string reply = sender + " :" + str + "\r\n";

	for (std::vector<Client>::iterator clientIt = _clients.begin();
		clientIt != _clients.end(); ++clientIt)
		{
			if (clientIt->getId() == id) {
				send(clientIt->getSocket(), reply.c_str(), reply.size(), 0);
				return ;
			}
		}
}
void	Server::sendToClient(int i, std::string str) {

	std::string reply = _clients[i].getNick() + " :" + str + "\r\n";
	send(_clients[i].getSocket(), reply.c_str(), reply.size(), 0);
}

void	Server::sendToClientsInChannel(int i, std::string str)
{
	int	channelId = _clients[i].getChannelId();
	if (channelId == -1)
		return ;
	std::string		channelName = _channels[channelId].getName();

	for (std::vector<Client>::iterator clientIt = _clients.begin();
		clientIt != _clients.end(); ++clientIt)//
		{
			if (clientIt->getChannelId() == channelId 
				&& clientIt->getId() != _clients[i].getId())
				{
					std::string sender = channelName + " :" + _clients[i].getNick();
					sendToClient(clientIt->getId(), sender, str);
				}
		}
}



int		Server::findOrCreateChannel(int i, std::string name)
{
	for (std::vector<Channel>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); ++channelIt)
	{
		if (name.substr(0, name.size() - 1) == channelIt->getName())
			return (channelIt->getId());
	}
	Channel temp(name.substr(0, name.size() - 1));
	_channels.push_back(temp);
	std::cout << _channels.rbegin()->getName() << " has been created" << std::endl;
	return (_channels.rbegin()->getId());
}
void	Server::commandJoin(int i, std::string name)
{
	int channelId = findOrCreateChannel(i, name);
	_clients[i].setChannelId(channelId + 1);
	std::cout << "Client " << _clients[i].getNick() << 
				" joined channel " << _channels[_clients[i].getChannelId()].getName() << std::endl;

	std::string strToSend = "JOIN " + _channels[_clients[i].getChannelId()].getName();//check if this is whats supposed to be said
	sendToClientsInChannel(i, strToSend);

	std::string welcomeMessage =  "Welcome to the channel, today's MOTD: " + _channels[_clients[i].getChannelId()].getName() + "temp motd!";//check if this is whats supposed to be said
	sendToClient(i, welcomeMessage);//THIS ONE IS NOT NEEDED, BUT ITS AN AGKNOWLEDGEMENT THAT CLIENT HAS JOINED
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
