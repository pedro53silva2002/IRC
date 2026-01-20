#include "Client.hpp"

//*Constructors
Client::Client() {
	_id = -1;
}
Client::Client(int srvSocket) {
	_id = _globalId++;
	_socket = srvSocket;
	_pfd.fd = _socket;
	_pfd.events = POLLIN;
	_pfd.revents = 0;
	_authenticated = false;
	_registered = false;
	_username = "";
	_nick = "*";
	_realname = "";
	_host = "";
	_prefix = "";
}
Client::Client(const Client& other) {
	*this = other;
}
Client& Client::operator=(const Client& other) {
	if (this != &other) {
		_id = other._id;
		_socket = other._socket;
		_pfd.fd = other._pfd.fd;
		_pfd.events = other._pfd.events;
		_pfd.revents = other._pfd.revents;
		_authenticated = other._authenticated;
		_registered = other._registered;
		_username = other._username;
		_nick = other._nick;
		_realname = other._realname;
		_host = other._host;
		_prefix = other._prefix;
		_chans = other._chans;
	}
	return (*this);
}


//*GETTERS
int			Client::getId() {
	return (_id);
}
int			Client::getSocket() {
	return (_socket);
}
pollfd		&Client::getPfd() {
	return (_pfd);
}

bool		Client::isAuthenticated() {
	return (_authenticated);
}
bool		Client::isRegistered() {
	return (_registered);
}
std::string	Client::getUsername() {
	return (_username);
}
std::string	Client::getNick() {
	return (_nick);
}
std::string	Client::getRealname() {
	return (_realname);
}
std::string	Client::getPrefix() {
	return (_prefix);
}
std::string	Client::getHost() {
	return (_host);
}
std::map<int, std::string>	&Client::getChannels() {
	return (_chans);
}

//*SETTERS
void	Client::setId(int id) {
	_id = id;
}
void	Client::setAuthenticated(bool auth) {
	_authenticated = auth;
}
void	Client::setRegistered(bool auth) {
	_registered = auth;
}
void	Client::setUsername(std::string username) {
	_username = username;
}
void	Client::setNick(std::string nick) {
	_nick = nick;
}
void	Client::setRealname(std::string realname) {
	_realname = realname;
}
void	Client::setPrefix() {
	_prefix = ":" + _nick + "!" + _username + "@127.0.0.1";
}
void	Client::setHost(std::string host) {
	_host = host;
}
void	Client::setChannel(int chId, std::string chName) {
	_chans.insert(std::make_pair(chId, chName));
}
