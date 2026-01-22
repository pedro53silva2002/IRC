#include "../includes/Server.hpp"

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
	if (line.compare(0, 6, "CAP LS") == 0)
		return ;
	
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
	
		for (int i = 1; i < _pfds.size(); i++)
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
