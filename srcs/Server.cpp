#include "../includes/Server.hpp"

//*CONSTRUCTORS
Server::Server() {
	_name = "just why did you try";
	throw (std::runtime_error(_name));
}
Server::Server(const Server& other) {
	*this = other;
}
Server& Server::operator=(const Server& other) {
	if (this != &other) {
		_name = other._name;
		_port = other._port;
		_pass = other._pass;
		_socket = other._socket;
		server_addr = other.server_addr;
		_srvPfd = other._srvPfd;
		_motd = other._motd;
		_pfds = other._pfds;
		_clients = other._clients;
		_channels = other._channels;
	}
	return (*this);
}
Server::~Server()
{
	serverLog("Server", "closing");
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
		close(it->second.getSocket());
	close(_socket);
}

Server::Server(char *port, char *pass)
{
	_name = SERVERNAME;
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

	_srvPfd.fd = _socket;
	_srvPfd.events = POLLIN;
	_srvPfd.revents = 0;
	_motd = "it is wednesday my dudes";
	std::cout << GREEN("Server open in port: ") << _port << std::endl;
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
	return (tempSocket);
}

void	setCommand(std::string line, std::string *command, std::string *args)
{
	std::istringstream iss(line);
	std::string temp;
	int i = 0;
	while (iss >> temp) {
		if (i == 0)
			*command = temp;
		else
			*args += temp + ' ';
		i++;
	}	
	*args = (*args).substr(0, (*args).length() - 1);
	std::transform((*command).begin(), (*command).end(), (*command).begin(), ::toupper);
	std::cout << "command input [" + *command + "], arguments: [" + *args + "]\n";
}

void	Server::processCommand(int i, std::string line)
{
	std::cout << RED("--------------------------------------------------------------------------------\n");
	std::cout << _clients[i].getNick() << " said: [" + line + "]\n";
	if (line.compare(0, 6, "CAP LS") == 0)//what to do
		return ;
	else if (line.compare(0, 3, "WHO") == 0)//what to do
		return ;

	typedef void (Server::*funcs)(int, std::string);
	std::string commands[] = {"QUIT", "PASS", "USER", "NICK", "JOIN",  "PART", "PRIVMSG", "KICK", "MODE", "TOPIC", "INVITE" };
	funcs function[] = {&Server::commandQuit, &Server::commandPass, &Server::commandUser, &Server::commandNick, &Server::commandJoin,  &Server::commandPart ,
		&Server::commandPrivmsg, &Server::commandKick, &Server::commandMode, &Server::commandTopic, &Server::commandInvite};

	std::string userCommand, args;
	setCommand(line, &userCommand, &args);
	for (int j = 0; j < 11; j++) {
		if (commands[j] == userCommand) {
			(this->*function[j])(i, args);
			return ;
		}
	}
	sendToClient(i, ERR_UNKNOWNCOMMAND(_clients[i].getNick(), line));
}

bool	Server::handleClientPoll(int i)
{
	static std::map<int, std::string> partBuf;
	char		buf[512];
	int bytesRecv = myRecv(i, buf, sizeof(buf), 0);
	if (bytesRecv == 0) {
		commandQuit(i, "");
		return (false);
	}
	buf[bytesRecv] = '\0';
	partBuf[i] += buf;

	size_t pos;
	while ((pos = partBuf[i].find("\r\n")) != std::string::npos) {
		std::string line = partBuf[i].substr(0, pos);
		partBuf[i].erase(0, pos + 2);
		processCommand(i, line);
	}
	return (true);
}

void	Server::srvRun()
{
	while (1)
	{
		setPfds();
		myPoll(_pfds.data(), _pfds.size(), -1);
		if (_pfds[0].revents & POLLIN)
		{
			int temp = acceptClient();
			_clients.insert(std::make_pair(temp, Client(temp)));
		}
		for (size_t i = 1; i < _pfds.size(); i++)
		{
			if (_pfds[i].revents & POLLIN) {
				int ret = handleClientPoll(_pfds[i].fd);
				if (ret == false)
					continue ;
			}
		}
	}
	close(_socket);
}
