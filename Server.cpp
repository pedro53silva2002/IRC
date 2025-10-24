#include "Server.hpp"

Server::Server() {
	_port = "";
	_password = "";
	_ServerSocket = 0;
	memset(&_hints, 0, sizeof(_hints));
	_hints.ai_family = AF_INET;          //IPv4
    _hints.ai_socktype = SOCK_STREAM;    //TCP socket
    _hints.ai_protocol = IPPROTO_TCP;    //explicitly TCP protocol
    _hints.ai_flags = AI_PASSIVE;  
	_res = NULL;
	_serverAddr = sockaddr_in();
	/* for (std::map<std::string, float>::iterator i = _dataBase.begin(); i != _dataBase.end(); ++i) 
        std::cout << i->first << " " << i->second << std::endl; */
	//std::cout << "BitcoinExchange default constructor called" << std::endl;
}

Server::Server(std::string port, std::string password) : _port(port), _password(password) {
	//CONFIGURATION OF NETWORK CONNECTION/ SETTING A SOCKET
	memset(&_hints, 0, sizeof(_hints));
	_hints.ai_family = AF_INET;          //IPv4
    _hints.ai_socktype = SOCK_STREAM;    //TCP socket
    _hints.ai_protocol = IPPROTO_TCP;    //explicitly TCP protocol
    _hints.ai_flags = AI_PASSIVE;        //used for listening sockets
	_res = NULL;
	_ServerSocket = 0;
	_serverAddr = sockaddr_in();
}

Server::Server(const Server& other)
{
	//std::cout << "BitcoinExchange copy assignation called" << std::endl;
	*this = other;
}

Server &Server::operator=(const Server &src)
{
	//std::cout << "BitcoinExchange assignation operator called" << std::endl;
	if (this != &src)
	{
		_port = src._port;
		_password = src._password;
		_res = src._res;
		_hints = src._hints;
		_ServerSocket = src._ServerSocket;
		_serverAddr = src._serverAddr;
	}
	return *this;
}

Server::~Server()
{
	//std::cout << "BitcoinExchange descontructor called" << std::endl;
}

std::string Server::getPort() const { return _port; }
std::string Server::getPassword() const { return _password; }
int Server::getServerSocket() const { return _ServerSocket; }
const struct addrinfo &Server::getHints() const { return _hints; }
struct addrinfo *Server::getRes() const { return _res; }
void Server::setRes(struct addrinfo *res) { _res = res; }
void Server::setServerSocket(int serverSocket) { _ServerSocket = serverSocket; }