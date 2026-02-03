#include "../includes/Server.hpp"

void	Server::commandPass(int i, std::string line)
{
	if (_clients[i].isAuthenticated())
		return (sendToClient(i, ERR_ALREADYREGISTERED(_clients[i].getNick())));
	if (line.compare(_pass))
		return (sendToClient(i, ERR_PASSWDMISMATCH(_clients[i].getNick())));
	_clients[i].setAuthenticated(true);
}

bool	Server::isValidUser(int i, std::string args)
{
	if (!_clients[i].isAuthenticated())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "USER")), false);
	if (!_clients[i].getUsername().empty() && !_clients[i].getRealname().empty())
		return (sendToClient(i, ERR_ALREADYREGISTERED(_clients[i].getNick())), false);
	int pos = 0;
	int newpos = 0;
	std::vector<std::string>	params;
	while (args.find(' ', pos) != std::string::npos)
	{
		newpos = args.find(' ', pos + 1);
		if (pos == 0)
			params.push_back(args.substr(pos, newpos));
		else
			params.push_back(args.substr(pos + 1, newpos - pos - 1));
		pos = newpos;
	}
	if (params.size() >= 4 && params[1][0] == '0' && params[2][0] == '*')	
		return (true);
	sendToClient(i, "WHAT SHOULD I PUT HERE");
	return (false);
}

void	Server::commandUser(int i, std::string args)
{
	if (!isValidUser(i, args))
		return ;

	int pos = 0;
	for (int i = 0; i < 4; ++i)
		pos = args.find(' ', pos + 1);
	std::string real = args.substr(pos + 1);
	std::string user = args.substr(0, args.find(' '));
	_clients[i].setRealname(real);
	_clients[i].setUsername(user);
	
	checkRegistration(i);
}

bool	Server::isValidNick(int i,std::string args)
{
	if (!_clients[i].isAuthenticated())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);	
	if (args.empty())
		return (sendToClient(i, ERR_NONICKNAMEGIVEN()), false);
	if (args.find(' ') != std::string::npos || args[0] == ':' || args[0] == '#' || args.compare(0, 2, "#&") == 0 || args.compare(0, 2, "&#") == 0)
		return (sendToClient(i, ERR_ERRONEUSNICKNAME(_clients[i].getNick(), args)), false);
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++)	{
		if (_clients[it->first].getNick() == args)
			return (sendToClient(i, ERR_NICKNAMEINUSE(_clients[i].getNick(), args)), false);
	}
	return (true);
}

void	Server::commandNick(int i, std::string args)
{
	if (!isValidNick(i, args))
		return ;
	std::string oldNick = _clients[i].getNick();
	_clients[i].setNick(args);
	if (_clients[i].isRegistered())
		serverBroadcast(NICK(oldNick, args));
	checkRegistration(i);
	_clients[i].setPrefix();
}


void	Server::welcomeClient(int i)
{
	sendToClient(i, "CAP * LS");
	sendToClient(i, RPL_WELCOME(_clients[i].getNick(), _name));
	//uncomment for full welcome
	/*
		sendToClient(i, RPL_YOURHOST);
		sendToClient(i, RPL_MYINFO(_clients[i].getNick()));
		sendToClient(i, RPL_MOTDSTART(_clients[i].getNick(), _name));
		sendToClient(i, RPL_MOTD(_clients[i].getNick(), _motd));
		sendToClient(i, RPL_ENDOFMOTD(_clients[i].getNick()));
	*/
}

void	Server::checkRegistration(int i)
{
	if (!_clients[i].isRegistered() && !_clients[i].getNick().empty() && !_clients[i].getUsername().empty() 
		&& !_clients[i].getRealname().empty() && _clients[i].getNick() != "*")
		{
			_clients[i].setRegistered(true);
			_clients[i].setPrefix();
			welcomeClient(i);
		}
}
