#include "Server.hpp"

Server::Server() {
	_port = "";
	_password = "";
	/* for (std::map<std::string, float>::iterator i = _dataBase.begin(); i != _dataBase.end(); ++i) 
        std::cout << i->first << " " << i->second << std::endl; */
	//std::cout << "BitcoinExchange default constructor called" << std::endl;
}

Server::Server(std::string port, std::string password) : _port(port), _password(password) {
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
	}
	return *this;
}

Server::~Server()
{
	//std::cout << "BitcoinExchange descontructor called" << std::endl;
}

std::string Server::getPort() const { return _port; }
std::string Server::getPassword() const { return _password; }