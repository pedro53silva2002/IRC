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

	//!VERY TEMPORARY
	// Channel temp("temp");//NAME WILL BE A PARAMETER FROM BUF
	// _channels.push_back(temp);
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
	std::vector<Client>::iterator it = _clients.begin();//stupid fix
	it++;
	while (it != _clients.end()){
		_pfds.push_back(it->getPfd());
		it++;
	}
}

//*Disconnect client when client exits
void	Server::commandQuit(int i, std::string str)//calling QUIT asks for reason, ctrl+c doesnt need reason
{
	serverLog(_clients[i].getNick(), "has disconnected");
	sendToClient(i, "QUIT :" + str);
	close (_pfds[i].fd);
	_clients.erase(_clients.begin() + i - 1);
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



void	Server::sendToClient(int i, std::string sender, std::string str)
{
	std::string reply = sender + " :" + str + "\r\n";
	send(_clients[i].getSocket(), reply.c_str(), reply.size(), 0);
}
void	Server::sendToClient(int i, std::string str) {
	sendToClient(i, _clients[i].getNick(), str);
}


void	Server::commandJoin(int i, std::string name)
{
	//first user will be op
	//from the on, users will be just users lol



	for (std::vector<Channel>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); ++channelIt)
	{
		//std::cout << "HERE: " << name.substr(0, name.size()) << " || " << channelIt->getName() << std::endl;
		if (name.substr(0, name.size()) == channelIt->getName())//PARSE AAAAAAAAA
		{
			_clients[i].setChannelId(channelIt->getId());
			std::cout << "Client " << _clients[i].getNick() << " joined channel " << channelIt->getName() << std::endl;
			return ;
		}
		//std::cout << "THERE: " << name.substr(0, name.size() - 1) << " || " << channelIt->getName() << std::endl;
	}
	Channel temp(name.substr(0, name.size() - 1));//PARSE AAAAAAAAA
	// std::cout << "temp name: " << temp.getName() << std::endl;
	_channels.push_back(temp);
	std::cout << "last channel name " << _channels.rbegin()->getName() << std::endl;
	std::cout << "id of channel that should be id 2: " << _channels.rbegin()->getId() << std::endl;//!WHY IS THE ID WRONG THE ID IS ALL WRONG
	_clients[i].setChannelId(_channels.rbegin()->getId());
	
	// std::cout << "Client " << _clients[i].getNick() << " created and joined channel " << _channels.rbegin()->getName() << std::endl;
}

void	Server::sendToClientsInChannel(int i, std::string str)
{
	//*for now hardcoded to stop non channeled clients from sending to clients
	int				channelId = _clients[i].getChannelId();
	std::cout << "Client nicked " << _clients[i].getNick() << " called this function and has channel id " << channelId << std::endl;
	if (channelId == -1)
		return ;
	std::string		channelName = _channels[channelId].getName();

	for (std::vector<Client>::iterator clientIt = _clients.begin();
		clientIt != _clients.end(); ++clientIt)
		{
			if (clientIt->getChannelId() == channelId) {
				std::cout << "Clients " << clientIt->getNick() << " will get the output" << std::endl;
				//DONT SEND THE MESSAGE BACK TO THE SENDER
				int socketToSendTo = clientIt - _clients.begin();
				std::string sender = channelName + " :" + _clients[i].getNick();
				sendToClient(socketToSendTo, sender, str);
			}
		}
}

void	Server::processCommand(int i)
{
	/*
		HAVE ALL COMMANDS BE CALLED HERE IN A SWITCH CASE
		EACH COMMAND FUNCTION WILL THEN CHECK IF ITS REGISTERED, AND DO WHAT NEEDS TO BE DONE IN CASE IT ISNT
		LIKE EXIT AND QUIT WONT CHECK
		PASS WONT CHECK
		USER AND NICK WILL CHECK ONLY IF ITS AUTHENTICATED
		ALL OTHERS WILL CHECK EVERYTHING
	*/
	// debugMessage(i);
	// sendToClient(_clients[i], _clients[i].getBuf());
	
	//*Closing server
	if (strncmp(_clients[i].getBuf(), "exit ", 4) == 0)
		exitServer();
	//*Disconnects client
	else if (strncmp(_clients[i].getBuf(), "QUIT ", 4) == 0)
		return (commandQuit(i, "hardcoded quit"));
	
	//*Registering client
	if (!_clients[i].isRegistered()) {
		registration(i);
		return ;
	}



	//*START OF CHANNEL LOGIC
	if (strncmp(_clients[i].getBuf(), "JOIN ", 5) == 0)
		commandJoin(i, _clients[i].getBuf() + 5);
	
	/*
		this function should only be called when other clients should know what happened. cases would be:
		non-op:	USER, NICK, PRIVMSG, more to be added;
		op: TOPIC, MODE, KICK, stuff like that to be checked later
	*/
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

	processCommand(i);//process command could be all here
	return (true);
}


void	Server::srvRun()
{
	std::cout << "ID FOR SENDTOCLIENTSINCHANNEL IS BAD\n";
	while (1)
	{
		
		setPfds();
		myPoll(_pfds.data(), _pfds.size(), -1);
		
		if (_pfds[0].revents & POLLIN)//* Client Connecting
		{
			int temp = acceptClient();
			_clients.push_back(Client(temp));

			Channel chan("FirstChannel");
			_channels.push_back(chan);
			/* //HARDCODED CLIENTS AND CHANNELS
			if (_clients.size() == 2) {
				std::cout << "First client\n";
				_clients[1].setAuthenticated(true);
				_clients[1].setRegistered(true);
				_clients[1].setNick("First");
				_clients[1].setUsername("First");
				_clients[1].setRealname("First");
				_clients[1].setChannelId(1);
				Channel temp("FirstChannel");
				_channels.push_back(temp);
				welcomeClient(1);
			}
			else if (_clients.size() == 3) {
				std::cout << "Second client\n";
				_clients[2].setAuthenticated(true);
				_clients[2].setRegistered(true);
				_clients[2].setNick("Second");
				_clients[2].setUsername("Second");
				_clients[2].setRealname("Second");
				// _clients[2].setChannelId(2);
				// Channel temp("SecondChannel");
				// _channels.push_back(temp);
				welcomeClient(2);
			}
			else if (_clients.size() == 4) {
				std::cout << "Third client\n";
				_clients[3].setAuthenticated(true);
				_clients[3].setRegistered(true);
				_clients[3].setNick("Third");
				_clients[3].setUsername("Third");
				_clients[3].setRealname("Third");
				// _clients[3].setChannelId(1);
				// Channel temp("FirstChannel");
				// _channels.push_back(temp);
				welcomeClient(3);
			} */
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
