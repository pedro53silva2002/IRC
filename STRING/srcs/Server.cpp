#include "../includes/Server.hpp"

//todo parse the main
//check order of parsing, like isOp, isInChannel
//todo if op leaves a channel, it gets removed. check what happens with op
//!change stuff for NEW instead of just pushback

/*
	PASS USER NICK 		FULLY done
	QUIT				//? should be done
	JOIN				//? done except key
	PART 				//? should be done
	PRIVMSG				//fully done?
	KICK 				//needs parsing
	MODE 				//needs parsing and outputs
	TOPIC 				//broken
	INVITE				//redone, needs parse
*/


/** @brief Constructs a Server object and initializes the listening socket.
 *  @param port - Port number on which the server will listen for incoming client connections.
 *  @param pass - Password required for clients to authenticate with the server.
*/
Server::Server(char *port, char *pass) {
	_name = "MyIRC";
	_port = atoi(port);
	_pass = pass;

	_socket = mySocket(AF_INET, SOCK_STREAM, 0);

	int opt = 1;
	setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(_port);
	inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr);

	myBind(_socket, (sockaddr*)&server_addr, sizeof(server_addr));

	myListen(_socket, SOMAXCONN);

	std::cout << GREEN("Server open in port: ") << _port << std::endl;

	_srvPfd.fd = _socket;
	_srvPfd.events = POLLIN;
	_srvPfd.revents = 0;

	_channels.push_back(Channel());//NEW
	_motd = "it is wednesday my dudes";
}



Server::~Server()
{
	serverLog("Server", "closing");
	for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++) {
		//delete the channels
	}
	//channels.clear();
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++) {
		close(it->second.getSocket());
	}
	//clients.clear();
	close(_socket);
}

int		Server::acceptClient()
{
	int			tempSocket;
	sockaddr_in	clientAddr;
	socklen_t	clientSize = sizeof(clientAddr);
	char		host[NI_MAXHOST];
	
	tempSocket = accept(_socket, (sockaddr*)&clientAddr, &clientSize);
	if (tempSocket == -1)
		throw (std::runtime_error("Problem with client connecting"));
	memset(host, 0, NI_MAXHOST);
	
	inet_ntop(AF_INET, &clientAddr.sin_addr, host, NI_MAXHOST);
	// std::cout << host << " manually connected on " << ntohs(clientAddr.sin_port) << std::endl;
	return (tempSocket);
}


//todo skip all whitespaces
std::string parseLine(std::string line)
{
	int pos = line.find(' ');
	if (pos == std::string::npos)
		return ("");
	std::string arguments = line.substr(pos + 1);
	return (arguments);
}


/** @brief Checks and select the command that was chosen from the input
 *  @param i - Index of the client that made the input
 *  @param line - Input in which the command is found
*/
void	Server::processCommand(int i, std::string line)
{
	std::cout << RED("--------------------------------------------------------------------------------\n");
	std::cout << _clients[i].getNick() << " said: [" + line + "]\n";
	if (line.compare(0, 6, "CAP LS") == 0)//todo figure out what to do
		return ;
	else if (line.compare(0, 4, "exit") == 0)
		throw (0);



	typedef void (Server::*funcs)(int, std::string);
	std::string commands[] = {"QUIT", "PASS", "USER", "NICK", "JOIN",  "PART", "PRIVMSG", "KICK", "MODE", "TOPIC"/* , "INVITE" */};
	funcs function[] = {&Server::commandQuit, &Server::commandPass, &Server::commandUser, &Server::commandNick, &Server::commandJoin,  &Server::commandPart ,
		&Server::commandPrivmsg, &Server::commandKick, &Server::commandMode, &Server::commandTopic/*, &Server::commandInvite*/};
	std::string temp = line.substr(0, line.find(' '));
	std::string args = parseLine(line);
	for (int j = 0; j < 11; j++) {
		if (commands[j] == temp) {
			(this->*function[j])(i, args);
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
	int bytesRecv = myRecv(i, buf, sizeof(buf), 0);
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

void	Server::testClients(int i)
{
	std::cout << "hardcoding client " << _clients[i].getId() << "\n";
	if (_clients.size() == 1) {
		_clients[i].setNick("First");
		_clients[i].setUsername("First");
		_clients[i].setRealname("First");
	}
	else if (_clients.size() == 2) {
		_clients[i].setNick("Second");
		_clients[i].setUsername("Second");
		_clients[i].setRealname("Second");
	}
	else if (_clients.size() == 3) {
		_clients[i].setNick("Third");
		_clients[i].setUsername("Third");
		_clients[i].setRealname("Third");
	}
	_clients[i].setAuthenticated(true);
	_clients[i].setRegistered(true);
	_clients[i].setHost(_name);
	_clients[i].setPrefix();
	welcomeClient(i);
}


void	Server::test()
{
	std::cout << RED("--------------------------------------------------------------------------------\n");
	serverLog("TESTING", "");
/* 	serverLog("Existing channels", "");
	for (int i = 0; i < _channels.size(); i++) {
		std::cout << i << ": [" << _channels[i].getName() << "], ";
	}
	std::cout << std::endl;
	serverLog("Existing clients", "");
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++)	{
		std::cout << it->first << ": [" << it->second.getNick() << "], ";
	}
	std::cout << std::endl; */
/* 	serverLog("Each client info:", "");
	for (std::map<int, Client>::iterator it1 = _clients.begin(); it1 != _clients.end(); it1++)	{
		std::cout << _clients[it1->first].getId() << ": [" << _clients[it1->first].getNick() << "] is connected to channels: ";
		
		for (std::map<int, std::string>::iterator it2 = _clients[it1->first].getChannels().begin(); 
		it2 != _clients[it1->first].getChannels().end(); it2++) {
			std::cout << it2->first << ": [" << it2->second << "], ";
		}
		std::cout << std::endl;
	} */
	serverLog("Each channel info:", "");
	for (int i = 0; i < _channels.size(); i++) {
		std::cout << i << ": [" << _channels[i].getName() << "] has these clients connected: ";
		for (std::vector<int>::iterator it = _channels[i].getClientsInChannel().begin(); 
			it != _channels[i].getClientsInChannel().end(); it++) {
				std::cout << "[" << _clients[*it].getNick() << "], ";
		}
		std::cout << std::endl;
	}
}

/** @brief Starts the server ready to accept the clients. */
void	Server::srvRun()
{
	while (1)
	{
		// test();
		setPfds();
		myPoll(_pfds.data(), _pfds.size(), -1);
		
		if (_pfds[0].revents & POLLIN)
		{
			int temp = acceptClient();
			_clients.insert(std::make_pair(temp, Client(temp)));

			//HARDCODED CLIENTS AND CHANNELS
			// testClients(temp);
		}
	
		for (int i = 1; i < _pfds.size(); i++)
		{
			if (_pfds[i].revents & POLLIN) {
				int ret = handleClientPoll(_pfds[i].fd);
				if (ret == false)//i dont like this
					continue ;
			}
		}
	}
	close(_socket);
}
