#include "../includes/Server.hpp"
//todo check the orders of errors

//*1 thing left, multiple params output
void	Server::commandPass(int i, std::string line)
{
	//just check for PASS <1st string> <2nd string>
	//CHECK ORDER OF NEEDMOREPARAMS VS ALREADYREGISTERED VS PASSWDMISMATCH
	if (_clients[i].isAuthenticated())
		return (sendToClient(i, ERR_ALREADYREGISTERED(_clients[i].getNick())));
	if (line.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "PASS")));
	if (line.compare(_pass))
		return (sendToClient(i, ERR_PASSWDMISMATCH(_clients[i].getNick())));
	_clients[i].setAuthenticated(true);
	serverLog(_clients[i].getNick(), "has authenticated");
}

//*should be done, just need to check is <nick> gets sent as a parameter here
bool	isValidUser(std::string line)
{
	int pos = 0;
	int newpos = 0;
	std::vector<std::string>	params;
	while (line.find(' ', pos) != std::string::npos)
	{
		newpos = line.find(' ', pos + 1);
		if (pos == 0)
			params.push_back(line.substr(pos, newpos));
		else
			params.push_back(line.substr(pos + 1, newpos - pos - 1));
		pos = newpos;
	}
	if (params.size() >= 4 && params[1][0] == '0' && params[2][0] == '*')	
		return (true);
	return (false);
}
void	Server::commandUser(int i, std::string line)
{
	// if (!_clients[i].isAuthenticated())//todo might not be needed to be here, check with Dot
	// 	return (sendToClient(i, ERR_NOTAUTH));
	if (!_clients[i].getUsername().empty() && !_clients[i].getRealname().empty())
		return (sendToClient(i, ERR_ALREADYREGISTERED(_clients[i].getNick())));
	if (line.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "USER")));
	if (!isValidUser(line))
		return (sendToClient(i, "WHAT SHOULD I PUT HERE"));

	int pos = 0;
	for (int i = 0; i < 4; ++i)
		pos = line.find(' ', pos + 1);
	std::string real = line.substr(pos + 1);
	std::string user = line.substr(0, line.find(' '));
	_clients[i].setRealname(real);
	_clients[i].setUsername(user);
		
	std::cout << _clients[i].getNick() << " set their username to: ";
	std::cout << _clients[i].getUsername() << " || " << _clients[i].getRealname() << std::endl;
	checkRegistration(i);
}

//*feels like its done, just a check of isRegistered
bool	isValidNick(std::string line)
{
	if (line.find(' ') != std::string::npos || line[0] == ':' || line[0] == '#' || line.compare(0, 2, "#&") == 0 || line.compare(0, 2, "&#") == 0)
		return (false);
	return true;
}
bool	Server::isNickInUse(std::string toFind)
{
	for (int i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getNick() == toFind)
			return true;
	}
	return false;
}
void	Server::commandNick(int i, std::string line)
{
	// if (!_clients[i].isAuthenticated())//todo might not be needed to be here, check with Dot
	// 	return (sendToClient(i, ERR_NOTAUTH));
	if (line.empty())
		return (sendToClient(i, ERR_NONICKNAMEGIVEN(_clients[i].getNick())));
	if (!isValidNick(line))
		return (sendToClient(i, ERR_ERRONEUSNICKNAME(_clients[i].getNick(), line)));
	if (isNickInUse(line))
		return (sendToClient(i, ERR_NICKNAMEINUSE(_clients[i].getNick(), line)));
	
	std::cout << _clients[i].getNick() << " set their nick to: ";
	_clients[i].setNick(line);
	std::cout << _clients[i].getNick() << std::endl;
	checkRegistration(i);

	//sendtoClientsInChannel("<old> changed nick to <new>"), only if client is connected to channel
}



//*Registration
void	Server::welcomeClient(int i)
{
	//remove this function, put it inside checkReg
	std::string welcome = "Welcome to the " + _name + " Network ";
		// + _clients[i].getNick() + "[!" + _clients[i].getUsername() 
		// + "@"+ "host" + "]";//hardcoded
	sendToClient(i, welcome);
	std::string todo = "The rest of the welcome message will come after";

	sendToClient(i, todo);
	//RPL_YOURHOST 
	//RPL_CREATED 
	//RPL_MYINFO
	//RPL_ISUPPORT (prob not needed)
	//LUSERS?
	//MOTD
}

//RENAME
void	Server::checkRegistration(int i)
{
	if (!_clients[i].isRegistered() && !_clients[i].getNick().empty() && !_clients[i].getUsername().empty() 
		&& !_clients[i].getRealname().empty() && _clients[i].getNick() != "*")
		{
			_clients[i].setRegistered(true);
			welcomeClient(i);
		}
}
