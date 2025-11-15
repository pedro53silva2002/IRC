#include "Client.hpp"

Client::Client() {
	_clientLen = sizeof(_clientAddr);
	_clientSocket = 0;
	/* for (std::map<std::string, float>::iterator i = _dataBase.begin(); i != _dataBase.end(); ++i) 
        std::cout << i->first << " " << i->second << std::endl; */
	//std::cout << "BitcoinExchange default constructor called" << std::endl;
	
	//accept should be outside?
	/* _socket = accept(srvSocket, (sockaddr*)&client, &clientSize);//study accept()
	if (_socket == -1)
		throw (std::runtime_error("Problem with client connecting")); */
	memset(_host, 0, NI_MAXHOST);
	//!save client_addr in client class
	//kinda want to connect them manually all the time
	/* int result = getnameinfo((sockaddr*)&client, clientSize, host, NI_MAXHOST, svc, NI_MAXSERV, 0);
	if (result)
		std::cout << host << " connected on " << svc << std::endl;
	else{//this seems stupid
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " manually connected on " << ntohs(client.sin_port) << std::endl;//study ntohs()
	} */
	_pfd.fd = 0;
	_pfd.events = POLLIN;
	_pfd.revents = 0;
}

/* Server::Server(std::string port, std::string password) : _port(port), _password(password) {
} */

Client::Client(const Client& other)
{
	//std::cout << "BitcoinExchange copy assignation called" << std::endl;
	*this = other;
}

Client &Client::operator=(const Client &src)
{
	//std::cout << "BitcoinExchange assignation operator called" << std::endl;
	if (this != &src)
	{
		_clientAddr = src._clientAddr;
		_clientLen = src._clientLen;
		_clientSocket = src._clientSocket;
	}
	return *this;
}

Client::~Client()
{
	//std::cout << "BitcoinExchange descontructor called" << std::endl;
}

struct sockaddr_in &Client::getClientAddr() { return _clientAddr; }
socklen_t &Client::getClientLen() { return _clientLen; }
int Client::getClientSocket() const { return _clientSocket; }
void Client::setClientSocket(int clientSocket) { _clientSocket = clientSocket; }
char *Client::getHost() { return _host; }
char *Client::getSvc() { return _svc; }
pollfd Client::getPfd() { return _pfd; }