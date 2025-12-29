#include "../includes/Server.hpp"

//What happens if a client leaves a channel or disconnects? does the channel disappear? or does it give op to some other person?
//check order of parsing, like isOp, isInChannel
//check using a non existent channel as a parameter of a command "MODE <nonexistent> +l 100"
//*HARDCODE CHANNELS WITH DIFFERENT MODES TO TEST EVERYTHING

/*
	PASS USER NICK 		done
	JOIN				//*redone, check key parsing and outputs
	PRIVMSG				//!doing
	PART 				//todo
	KICK 				//todo
	INVITE				//todo
	MODE 				//todo
	TOPIC 				//todo
	QUIT				//todo
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
	_channels.push_back(Channel());//This is so that we dont have to work with _channel[j - 1]
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
	memset(host, 0, NI_MAXHOST);
	
	inet_ntop(AF_INET, &clientAddr.sin_addr, host, NI_MAXHOST);
	std::cout << host << " manually connected on " << ntohs(clientAddr.sin_port) << std::endl;
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


void	Server::processCommand(int i, std::string line)
{
	if (line.compare(0, 11, "CAP LS 302") == 0)//todo figure out what to do
		return ;
	if (line.compare(0, 4, "QUIT") == 0)
		return commandQuit(i, "HARDCODED");
	else if (line.compare(0, 4, "exit") == 0)
		return exitServer();
	else if (line.compare(0, 4, "test") == 0)
		return clientBroadcast(i, "FIRST", "THIS IS A TEST MESSAGE");



	
	typedef void (Server::*funcs)(int, std::string);
	std::string commands[] = {"PASS", "USER", "NICK", "JOIN"/* , "PART" */, "PRIVMSG", "KICK", "INVITE", "MODE", "TOPIC", "QUIT"};

	funcs function[] = {&Server::commandPass, &Server::commandUser, &Server::commandNick, &Server::commandJoin, /* &Server::commandPart ,*/
	&Server::commandPrivmsg/* , &Server::commandKick, &Server::commandInvite, &Server::commandMode, &Server::commandTopic, &Server::commandQuit */};
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
		_clients[1].setChannel(1, "FIRST");
		_channels.push_back(Channel("FIRST"));
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
		// _clients[2].setChannel(1, "FIRST");
		welcomeClient(2);
	}
	// else if (_clients.size() == 4) {
	// 	_clients[3].setAuthenticated(true);
	// 	_clients[3].setRegistered(true);
	// 	_clients[3].setNick("Third");
	// 	_clients[3].setUsername("Third");
	// 	_clients[3].setRealname("Third");
	// 	_clients[3].setPrefix(setPrefixTemp(3));
	// 	// _clients[3].setChannel(1, "FIRST");
	// 	welcomeClient(3);
	// }
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
