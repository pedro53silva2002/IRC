#include "../includes/Server.hpp"

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

Server::Server(char *port, char *pass)
{
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

	_motd = "it is wednesday my dudes";
}


Server::~Server()
{
	serverLog("Server", "closing");
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++) {
		close(it->second.getSocket());
	}
	close(_socket);
}

/**
 * @brief Accepts a new incoming client connection.
 * 
 * Waits for a new client to connect, accepts the connection,
 * and returns the new client's socket file descriptor.
 * 
 * @return The socket file descriptor for the accepted client.
 * 
 * @throws std::runtime_error if accepting the client fails.
 * 
 */
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

/**
 * @brief Extracts the arguments from a command line string.
 * 
 * Finds the first space in the input string and returns the substring
 * following it, which represents the command arguments.
 * 
 * @param line The full command line string.
 * 
 * @return The arguments substring after the first space, or an empty string if none found.
 */
std::string parseLine(std::string line)
{
	size_t pos = line.find(' ');
	if (pos == std::string::npos)
		return ("");
	std::string arguments = line.substr(pos + 1);
	return (arguments);
}


/**
 * @brief Processes a command received from a client.
 * 
 * Parses the command line to identify the command and its arguments,
 * then invokes the corresponding handler function for that command.
 * 
 * @param i    The file descriptor index of the client sending the command.
 * @param line The full command line string received from the client.
 * 
 * @note If the command is unrecognized, sends an ERR_UNKNOWNCOMMAND response to the client.
 */
void	Server::processCommand(int i, std::string line)
{
	// std::cout << _clients[i].getNick() << " said: [" + line + "]\n";
	if (line.compare(0, 6, "CAP LS") == 0)
		return ;
	if (line.compare(0, 3, "WHO") == 0)
		return ;
	else if (line.compare(0, 4, "exit") == 0)
		throw (0);

	typedef void (Server::*funcs)(int, std::string);
	std::string commands[] = {"QUIT", "PASS", "USER", "NICK", "JOIN",  "PART", "PRIVMSG", "KICK", "MODE", "TOPIC", "INVITE" };
	funcs function[] = {&Server::commandQuit, &Server::commandPass, &Server::commandUser, &Server::commandNick, &Server::commandJoin,  &Server::commandPart ,
		&Server::commandPrivmsg, &Server::commandKick, &Server::commandMode, &Server::commandTopic, &Server::commandInvite};
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

/**
 * @brief Handles incoming data from a client socket.
 * 
 * Receives data from the client, processes complete command lines,
 * and handles client disconnection if necessary.
 * 
 * @param i The file descriptor index of the client.
 * 
 * @return true if the client is still connected, false if the client disconnected.
 * 
 * @note Calls processCommand() for each complete command line received.
 * @note Calls commandQuit() and returns false if the client disconnects.
 */
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
	serverLog("Each client info:", "");
	for (std::map<int, Client>::iterator it1 = _clients.begin(); it1 != _clients.end(); it1++)	{
		std::cout << _clients[it1->first].getId() << ": [" << _clients[it1->first].getNick() << "] is connected to channels: ";
		
		for (std::map<int, std::string>::iterator it2 = _clients[it1->first].getChannels().begin(); 
		it2 != _clients[it1->first].getChannels().end(); it2++) {
			std::cout << it2->first << ": [" << it2->second << "], ";
		}
		std::cout << std::endl;
	}
/* 	serverLog("Each channel info:", "");
	for (size_t i = 0; i < _channels.size(); i++) {
		std::cout << i << ": [" << _channels[i].getName() << "] has these clients connected: ";
		for (std::vector<int>::iterator it = _channels[i].getClientsInChannel().begin(); 
			it != _channels[i].getClientsInChannel().end(); it++) {
				std::cout << "[" << _clients[*it].getNick() << "], ";
		}
		std::cout << std::endl;
	} */
}


/**
 * @brief Main server loop for handling client connections and events.
 * 
 * Continuously polls for new client connections and incoming data from clients,
 * accepts new clients, and dispatches events to the appropriate handlers.
 * 
 * @note Calls test() for debugging output each loop iteration.
 * @note Uses setPfds() to update the pollfd vector.
 * @note Accepts new clients and adds them to the _clients map.
 * @note Handles incoming data for each client using handleClientPoll().
 * @note Closes the server socket on exit.
 */
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
