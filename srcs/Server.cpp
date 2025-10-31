#include "../includes/Server.hpp"

//*CONSTRUCTORS
Server::Server(char *port, char *pass){
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

	std::cout << "Server open in port: " << _port << std::endl;
	// std::cout << GREEN("Server open in port: ") << _port << std::endl;

	_srvPfd.fd = _socket;
	_srvPfd.events = POLLIN;
	_srvPfd.revents = 0;

	_clients.push_back(Client());//This is so that we dont have to work with _clients[i - 1]
}

//*Accepting client
int		Server::acceptClient()
{
	int			tempSocket;
	sockaddr_in	clientAddr;
	socklen_t	clientSize = sizeof(clientAddr);
	char		host[NI_MAXHOST];
	
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
	std::vector<Client>::iterator it = _clients.begin();//stupid fucking fix
	it++;
	while (it != _clients.end()){
		_pfds.push_back(it->getPfd());
		it++;
	}
}

//*Disconnect client when client exits
void	Server::disconnectClient(Client client, int i)
{
	//remove client
	std::cout << "Client " << client.getUsername() << " disconnected" << std::endl;

	//use getID to make it one less parameter
	close (_pfds[i].fd);
	_clients.erase(_clients.begin() + i - 1);
}

//this is just testing
bool	Server::shouldServerExit(char buf[])
{
	if (strncmp(buf, "exit", 4) == 0) {
		std::cout << "exiting server" << std::endl;
		for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
			close(it->getSocket());
		close(_socket);
		return (true);
	}
	return (false);
}


//debug message, replace by printInfo when done
void	debugClientMessage(Client client, char buf[])
{
	std::cout << "Client " << client.getId() << " said: " << buf;
}	


enum	pollCondition//fucking stupid find a better solution
{
	DISCONNECT,
	EXIT,
	OK
};


//*Parsing i think, coletes double check
std::string getUsername(const std::string &line) {
    size_t pos = 0;
    for (int i = 0; i < 1; ++i)
        pos = line.find(' ', pos + 1);
    return line.substr(pos + 1, line.find(' ', pos + 2) - pos - 1);
}

std::string getRealname(const std::string &line) {
    size_t pos = 0;
    for (int i = 0; i < 4; ++i)
        pos = line.find(' ', pos + 1);
    return line.substr(pos + 1);
}

std::string getNick(const std::string &line) {
    size_t pos = 0;
	std::string nickname;
    for (int i = 0; i < 1; ++i)
        pos = line.find(' ', pos + 1);
	nickname = line.substr(pos + 1, line.find(' ', pos + 2) - pos - 1);
	if (nickname[0] == ':' || nickname == "#" || !strncmp(nickname.c_str(), "#&", 2) || !strncmp(nickname.c_str(), "&#", 2) || nickname.empty())
		return ("");
	for (size_t i = 0; i < nickname.size(); i++)
	{
		if (nickname[i] == ' ')
			return ("");
	}
    return (nickname);
}

std::string kickUser(const std::string &line, std::vector<Client> &clients, std::vector<pollfd> &_pfds) {
    size_t pos = 0;
	std::string userToKick;
    for (int i = 0; i < 1; ++i)
        pos = line.find(' ', pos + 1);
	userToKick = line.substr(pos + 1, line.find(' ', pos + 1) - pos);
	userToKick[userToKick.size() - 2] = '\0'; // Remove trailing \r\n
	/* for (std::vector<Client>::iterator client_it = clients.begin(); client_it != clients.end(); client_it++)
		if (client_it->getUsername() == userToKick)
		{
			close(client_it->getSocket());
			clients.erase(client_it);
			break ;
		} */
	// Find and remove the client safely
    for (std::vector<Client>::iterator it = clients.begin() + 1; it != clients.end(); ++it) {
		std::cout << "SEARCH: " << userToKick << "\t\tCOMPARISON: " << it->getUsername() << " | " << strcmp(it->getUsername().c_str(), userToKick.c_str()) << std::endl;
        if (!strcmp(it->getUsername().c_str(), userToKick.c_str())) {
			std::cout << "GOT HERE\n" << std::endl;
            //send(it->getSocket(), "You were kicked\r\n", 19, 0);
            close(it->getSocket());
			_pfds.erase(_pfds.begin() + std::distance(clients.begin(), it)); // Remove from pollfds
            clients.erase(it);  // Erase removes them safely
            break;              // Stop — iterator is now invalid
        }
    }

    return (userToKick);
}











int	Server::handleClientPoll(int i)//i here is only sent so that _pfds and _clients are at the same index
{
	char buf[512];//this can change for a vector if needed
	int bytesRecv = myRecv(_pfds[i].fd, buf, sizeof(buf), 0);
	if (bytesRecv == 0) {
		disconnectClient(_clients[i], i);
		return (DISCONNECT);
	}
	buf[bytesRecv] = 0;
	// debugClientMessage(_clients[i], buf);

	if (shouldServerExit(buf))
		return (EXIT);

	_clients[i].setBuf(buf);

		//Vou por os comandos a serem processados assim, depois vemos isto juntos
		/* 
			tryPass(extractedPass) {
				if (extractedPass != _pass)
					throw WRONGPASSstd::vector<Client> _clients
			processCommand() {
				if (!clients.isauth())
					tryAuthClient()
				else {
					Any other command, like USER or NICK or JOIN or PRIVMSG or whatever
				}
			}
			
			AQUI

			try
				processCommand
			catch
				cout "command error"

				(check :IRC (RFC 1459 / 2812) for what to do in each case)

		*/


	

	if (!_clients[i].isAuthenticated())
	{
		char *bufPass = buf/* strncpy(buf, bufPass, bytesRecv) */;
		bufPass[bytesRecv - 2] = '\0'; //removing \r\n
		//bufPass[bytesRecv - 1] = '\0'; //removing \r\n
		std::cout << "Mine: " << bufPass << "\tOrig: " << _pass << "\tValue: " << (bufPass == _pass) << std::endl;
		if (!strncmp(buf, "PASS ", 5)/* strcmp(bufPass, &pass[0]) == 0 */)
		{
			std::string line(bufPass);
			size_t pos = 0;
			for (int i = 0; i < 1; ++i)
				pos = line.find(' ', pos + 1);
			std::cout << "Extracted pass: " << line/* .substr(pos + 1) */ << std::endl;	
			if (strcmp(line.substr(pos + 1).c_str(), _pass.c_str()) == 0)
			{
				std::cout << "Password correct\nClient should now be able to set user and talk\n";
				_clients[i].setAuthenticated(true);
			}
			else {
				//std::cout << _clients[i].getUsername() << " Password incorrect\n";
				throw std::runtime_error(_clients[i].getUsername() + ": Password incorrect\n");
			}
		}
		else {
			std::cout << "Client cannot talk\n";
		}
	}
	else
	{
		//std::cout << "Client " << _clients[i].getUsername()<< " said: " << buf;
		if (strncmp(buf, "USER ", 5) == 0)
		{
			_clients[i].setUsername(getUsername(buf));
			_clients[i].setRealname(getRealname(buf));
			std::cout << "Username set to: " << _clients[i].getUsername() << " || " << _clients[i].getRealname() << std::endl;
		}
		if (strncmp(buf, "NICK ", 5) == 0)
		{
			_clients[i].setNick(getNick(buf));
			std::cout << "Nick set to: " << _clients[i].getNick() << std::endl;
		}
		if (strncmp(buf, "KICK ", 5) == 0)
		{
			std::string userToKick = kickUser(buf, _clients, _pfds);
			std::cout << "User " << userToKick << " kicked." << std::endl;
		}
		if (strncmp(buf, "JOIN ", 5) == 0)
		{
			std::cout << "User " << _clients[i].getUsername() << " joined the chat." << std::endl;
		}
		if (strncmp(buf, "INVITE ", 5) == 0)
		{
			std::cout << "User " << _clients[i].getUsername() << " got invited" << std::endl;
		}
		if (strncmp(buf, "TOPIC ", 5) == 0)
		{
			std::cout << "User " << _clients[i].getUsername() << " changed the topic" << std::endl;
		}
		if (strncmp(buf, "QUIT", 4) == 0)
		{
			return (EXIT);
		}
	}

	return (OK);
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
				if (ret == DISCONNECT)//i dont like this
					continue ;
				else if (ret == EXIT)
					return ;
			}
		}
	}
	close(_socket);
}



/* 
	Have a function called printInfo, that will have client id, nick and/or user, authenticated, and the command
	maybe for this, buffer or command will have to be saved onto client

	also, hard code a client with info already in it and pass a command as if it was written to test the parsing
*/
