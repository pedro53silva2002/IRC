#include "../includes/Server.hpp"
#include "../includes/Client.hpp"

//*GETTERS FROM LINE
std::string getUsername(const std::string &line) {
	size_t pos = 0;
    for (int i = 0; i < 1; ++i)
        pos = line.find(' ', pos + 1);
    return line.substr(pos + 1, line.find(' ', pos + 2) - pos - 1);
    /* size_t pos = 0;
    for (int i = 0; i < 1; ++i)
        pos = line.find(' ', pos + 1);
	std::string username = line.substr(pos + 1, line.find(' ', pos + 2) - pos - 1);
	while (!username.empty() && (username[username.size() - 1] == '\r' || username[username.size() - 1] == '\n'))
		username.erase(username.size() - 1);
    return (username); */
}
std::string getRealname(const std::string &line) {
	/* size_t pos = 0;
    for (int i = 0; i < 4; ++i)
        pos = line.find(' ', pos + 1);
    return line.substr(pos + 1); */
    size_t pos = 0;
    for (int i = 0; i < 4; ++i)
        pos = line.find(' ', pos + 1);
	std::string realname = line.substr(pos + 1);
	while (!realname.empty() && (realname[realname.size() - 1] == '\r' || realname[realname.size() - 1] == '\n'))
		realname.erase(realname.size() - 1);
    return (realname);
}
std::string getNick(const std::string &line) {
	// size_t pos = 0;
	// std::string nickname;
    // for (int i = 0; i < 1; ++i)
    //     pos = line.find(' ', pos + 1);
	// nickname = line.substr(pos + 1, line.find(' ', pos + 2) - pos - 1);
	// if (nickname[0] == ':' || nickname == "#" || !strncmp(nickname.c_str(), "#&", 2) || !strncmp(nickname.c_str(), "&#", 2) || nickname.empty())
	// 	return ("");
	// for (size_t i = 0; i < nickname.size(); i++)
	// {
	// 	if (nickname[i] == ' ')
	// 		return ("");
	// }
    // return (nickname);
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

	while (!nickname.empty() && (nickname[nickname.size() - 1] == '\r' || nickname[nickname.size() - 1] == '\n'))
		nickname.erase(nickname.size() - 1);
	return (nickname);
}


//*Registration Commands
void	Server::commandPass(int i, std::string line)//*1 thing left
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

void	Server::commandUser(int i, std::string line)
{
	// if (ft_UserCommand(_clients[i].getBuf()))//!COLETES FIX

	if (!_clients[i].isAuthenticated())//todo might not be needed to be here, check with Dot
		return (sendToClient(i, ERR_NOTAUTH));

	if (!_clients[i].getUsername().empty() && !_clients[i].getRealname().empty())
		return (sendToClient(i, ERR_ALREADYREGISTERED(_clients[i].getNick())));

	if (line.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "USER")));

	std::cout << _clients[i].getNick() << " set their username to: ";
	_clients[i].setUsername(getUsername(line));
	_clients[i].setRealname(getRealname(line));
	std::cout << _clients[i].getUsername() << " || " << _clients[i].getRealname() << std::endl;
	checkRegistration(i);
}

//ERR_NICKNAMEINUSE

bool	isValidNick(std::string line)
{
	int pos = 0;
	int newpos = 0;
	std::vector<std::string>	params;
	while (line.find(' ', pos) != std::string::npos)//remake this
	{
		newpos = line.find(' ', pos + 1);
		if (pos == 0)
			params.push_back(line.substr(pos, newpos));
		else
			params.push_back(line.substr(pos + 1, newpos - pos));
		pos = newpos;
	}
	if (params.size() == 2)	
		return (true);
	return (false);
}

void	Server::commandNick(int i, std::string line)
{

	if (!_clients[i].isAuthenticated())//todo might not be needed to be here, check with Dot
		return (sendToClient(i, ERR_NOTAUTH));
	if (line.empty())
		return (sendToClient(i, ERR_NONICKNAMEGIVEN(_clients[i].getNick())));
	if (!isValidNick(line))
		return (sendToClient(i, ERR_ERRONEUSNICKNAME(_clients[i].getNick(), line)));
	//if (nick in channel) -> ERR_NICKNAMEINUSE
	
	std::cout << _clients[i].getNick() << " set their nick to: ";
	_clients[i].setNick(getNick(line));
	std::cout << _clients[i].getNick() << std::endl;
	//todo if statement for switching NICK after registering
	checkRegistration(i);


	/* 
		if NICK is set at registration, run checkRegistration
		if it is after that and the client is in a channel, sendtoclientsinchannel("<old> changed nick to <new>")
	*/
}

bool	ft_PassComand(char *buf)
{
	int pos = 0;
	int newpos = 0;
	std::string line(buf);
	std::vector<std::string>	params;
	if (!strncmp(buf, "PASS ", 5))
	{
		while (line.find(' ', pos) != std::string::npos)
		{
			newpos = line.find(' ', pos + 1);
			if (pos == 0)
				params.push_back(line.substr(pos, newpos));
			else
				params.push_back(line.substr(pos + 1, newpos - pos));
			pos = newpos;
		}
		/* std::cout << "PARAMS: size " << params.size() << std::endl;
		for (size_t i = 0; i < params.size(); i++)
		{
			std::cout << params[i] << "\n";
		} */
		if (params.size() == 2)	
			return (true);
	}
	return (false);
}
bool	ft_UserCommand(char *buf)
{
	int pos = 0;
	int newpos = 0;
	std::string line(buf);
	std::vector<std::string>	params;
	if (!strncmp(buf, "USER ", 5))
	{
		while (line.find(' ', pos) != std::string::npos)
		{
			newpos = line.find(' ', pos + 1);
			if (pos == 0)
				params.push_back(line.substr(pos, newpos));
			else
				params.push_back(line.substr(pos + 1, newpos - pos - 1));
			pos = newpos;
		}
		/* std::cout << params.size() << "\tPARAMS: " << strcmp(params[2].c_str(), "0") << " | " << strcmp(params[3].c_str(), "*") << std::endl;
		for (size_t i = 0; i < params.size(); i++)
		{
			std::cout << params[i] << "\n";
		} */
		if (params.size() >= 5 && strcmp(params[2].c_str(), "0") == 0 && strcmp(params[3].c_str(), "*") == 0)	
			return (true);
	}
	return (false);
}


//*Registration
void	Server::welcomeClient(int i)
{
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
void	Server::checkRegistration(int i)
{
	if (!_clients[i].getNick().empty() && !_clients[i].getUsername().empty() && !_clients[i].getRealname().empty() && _clients[i].getNick() != "*")
	{
		_clients[i].setRegistered(true);
		welcomeClient(i);
	}
}
