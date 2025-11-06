#include "../includes/Server.hpp"

//todo do the isRegistered() part and the welcome part, then remove all comments and make it readable, then channels!!!

//todo Channel logic
/* 
when a message is sent to server, save the channel that the client who sent it used, 
then go to that channels ClientsInChannel, then loop through getId to find which clients to sendInfo to




	class channel
	{
		vector ClientsInChannel (saves their id)
		
	}
	
	
	Server
	{
		vector Channel
	}


	


	WHERE WOULD THIS GO THO????
	SHALL I JUST TEST IT AFTER PROCESSING REGISTRATION???
	while (_clients[i]) 
	{
		if (_clients[i].getId() == ClientsInChannel)      FIND INSIDE THE VECTOR OF ClientsInChannel
		{
			sendToClient(_clients[i], LINE_TO_SEND);
		}
	}


	commandJoin(std::string str) {
		if (Channel with name str doesnt exist)
		{
			createChannel()
			save _clients[i].getId() into channel.ClientsInChannel
		}
		else
		{
			save _clients[i].getId() into channel.ClientsInChannel
		}
	}



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

	//!VERY TEMPORARY
	Channel temp("temp");//NAME WILL BE A PARAMETER FROM BUF
	_channels.push_back(temp);
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
	sendToClient(_clients[i], "QUIT :" + str);
	close (_pfds[i].fd);
	_clients.erase(_clients.begin() + i - 1);
}


//this is just testing
int	Server::exitServer()
{
	std::cout << "exiting server" << std::endl;
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
		close(it->getSocket());
	close(_socket);
	return (0);
}




void	sendToClient(Client client, std::string str) {
	std::string reply = ":<FIXED_SENDER> :" + str + "\r\n";
	send(client.getSocket(), reply.c_str(), reply.size(), 0);
}




//!THIS IS SO BAD HONESTLY
void	Server::commandJoin(int i)
{
	//before creating a new channel, it will check if there is one with that name
	// Channel temp("temp");//NAME WILL BE A PARAMETER FROM BUF
	// _channels.push_back(temp);

	_channels[0].clientJoin(_clients[i].getId());
	
}


//!THIS IS BAD BECAUSE IT SENDS TO THE RIGHT CLIENT BUT THE WRONG NICKNAME, HOW TF DO I CORRECT THIS
void	Channel::sendToClientsInChannel(std::vector<Client> clients, std::string str) {
	for (std::vector<int>::iterator idIt = _clientsInChannel.begin(); idIt != _clientsInChannel.end(); idIt++)
	{
		int id = *idIt;
		for (std::vector<Client>::iterator clientIt = clients.begin();
			clientIt != clients.end(); ++clientIt) {
				if (clientIt->getId() == id)
				{
					sendToClient(*clientIt, str);
					break ;
				}
			}
	}
}




// enum	ClientStatus
// {
// 	NOT_AUTHED,
// 	NOT_REGISTERED,
// 	REGISTERED,
// 	QUITTING,
// 	EXITING_SRV
// };
// int		Server::getStatus(int i)
// {
// 	if (!_clients[i].isAuthenticated())
// 		return NOT_AUTHED;
// 	else if (!_clients[i].isRegistered())
// 		return NOT_REGISTERED;
// 	else if (strncmp(_clients[i].getBuf(), "QUIT ", 4) == 0)
// 		return QUITTING;
// 	else if (strncmp(_clients[i].getBuf(), "exit ", 4) == 0)
// 		return EXITING_SRV;
// }


void	Server::processCommand(int i)
{
	// std::cout << YELLOW("Debug: ") << "Client " << _clients[i].getNick()<< " said: " << _clients[i].getBuf();
	
	//*Closing server
	if (strncmp(_clients[i].getBuf(), "exit ", 4) == 0)
		throw (exitServer());
	//*Disconnects client
	else if (strncmp(_clients[i].getBuf(), "QUIT ", 4) == 0)
		return (commandQuit(i, "hardcoded quit"));
	
	//*Registering client
	if (!_clients[i].isRegistered()) {
		registration(i);
		return ;
	}


	//*echo
	sendToClient(_clients[i], _clients[i].getBuf());

	//!THIS IS SO BAD HONESTLY
	// if (strncmp(_clients[i].getBuf(), "JOIN ", 5) == 0)
	// 	commandJoin(i);
	// _channels[0].sendToClientsInChannel(_clients, _clients[i].getBuf());
	//!THIS IS SO BAD HONESTLY
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


void	Server::srvRun()
{
	while (1)
	{
		setPfds();

		myPoll(_pfds.data(), _pfds.size(), -1);

		if (_pfds[0].revents & POLLIN){
			int temp = acceptClient();
			_clients.push_back(Client(temp));
		}//* Client Connecting
	
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
