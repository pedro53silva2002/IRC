#include "../includes/Server.hpp"
#include "../includes/Client.hpp"

bool	ft_NickComand(char *buf)
{
	int pos = 0;
	int newpos = 0;
	std::string line(buf);
	std::vector<std::string>	params;
	if (!strncmp(buf, "NICK ", 5))
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


//CASES WHERE NAMES ARE EMPTY OR DUPLICATE, OR USER IS TRIED TO BE SET AGAIN, THERES OUTPUTS TO PUT

void	Server::registration(int i, std::string line)
{
	// if (!_clients[i].isCapped())
	// {
	// 	if (strncmp(_clients[i].getBuf(), "CAP ", 4) == 0)
	// 	{
	// 		//std::cout << "CAPPED: " << _clients[i].isCapped() << "AUTHENTICATED: " << _clients[i].isAuthenticated() << std::endl;
	// 		std::string line(_clients[i].getBuf());
	// 		if (line.find("LS") != std::string::npos)
	// 		{
	// 			std::cout << ":localhost CAP * LS :multi-prefix sasl\n";
	// 			/* std::string response = ":localhost CAP * LS :multi-prefix sasl\n";
	// 			send(_clients[i].getSocket(), response.c_str(), response.size(), 0); */
	// 		}
	// 		else if (line.find("REQ") != std::string::npos)
	// 		{
	// 			std::cout << ":localhost CAP * ACK :multi-prefix\n";
	// 			/* std::string response = ":localhost CAP * ACK :multi-prefix\n";
	// 			send(_clients[i].getSocket(), response.c_str(), response.size(), 0); */
	// 		}
	// 		else if (line.find("END") != std::string::npos)
	// 		{
	// 			_clients[i].setCapped(true);
	// 			//std::cout << "CAPPED: " << _clients[i].isCapped() << "AUTHENTICATED: " << _clients[i].isAuthenticated() << std::endl;
	// 		}
	// 		else
	// 		{
	// 			std::cout << "U gotta put a good cap command...... no cap\n";
	// 			/* std::string response = "U gotta put a good cap command...... no cap\n";
	// 			send(_clients[i].getSocket(), response.c_str(), response.size(), 0); */
	// 		}
	// 	}
	// 	else if (strncmp(_clients[i].getBuf(), "CAP ", 4) != 0)
	// 	{
	// 		std::cout << "ONLY CAP!!!\n";
	// 		/* std::string response = "ONLY CAP!!!\n";
	// 		send(_clients[i].getSocket(), response.c_str(), response.size(), 0); */
	// 	}
	// }
	
	// std::string line;
	// if (line.compare(0, 10, "CAP LS 302") == 0)
	// 	return (sendToClient(i, "ircserv CAP * LS :"));//!needs checking
		
	if (!_clients[i].isAuthenticated())
		return (authClient(i, line));

	// if (ft_UserCommand(_clients[i].getBuf()))//!COLETES FIX
	if (line.compare(0, 5, "USER ") == 0)
		registerUser(i, line);
	// else if (ft_NickComand(_clients[i].getBuf()))//!COLETES FIX
	else if (line.compare(0, 5, "NICK ") == 0)
		registerNick(i, line);
	else {
		sendToClient(i, ERR_NOTREGISTERED);
		serverLog(_clients[i].getNick(), "not registered, cant talk");
	}
}



//*Authenticating
void	Server::authClient(int i, std::string line)
{
	// if (!ft_PassComand(_clients[i].getBuf()))//!COLETES FIX
	if (line.compare(0, 5, "PASS "))
	{
		// sendToClient(i, ERR_NOTAUTH);
		return (serverLog(_clients[i].getNick(), "is not authenticated, cannot talk"));
	}
	line = line.substr(5);
	if (line.compare(0, line.size(), _pass))
	{
		sendToClient(i, ERR_PASSWDMISMATCH);
		return (serverLog(_clients[i].getNick(), "guessed the password wrong"));
	}

	_clients[i].setAuthenticated(true);
	// sendToClient(i, PASSACCEPT);
	serverLog(_clients[i].getNick(), "has authenticated, needs to register");
}



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

//*Registration
void	Server::welcomeClient(int i)
{
	std::string welcome = "Welcome to the " + _name + " Network, "
		+ _clients[i].getNick() + "[!" + _clients[i].getUsername() 
		+ "@"+ "host" + "]";//hardcoded
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

void	Server::registerUser(int i, std::string line)//should be client class, not server class
{
	std::cout << _clients[i].getNick() << " set their username to: ";//check if here is getNick
	_clients[i].setUsername(getUsername(line));
	_clients[i].setRealname(getRealname(line));
	std::cout << _clients[i].getUsername() << " || " << _clients[i].getRealname() << std::endl;
	checkRegistration(i);
}
void	Server::registerNick(int i, std::string line)//should be client class, not server class
{
	std::cout << _clients[i].getNick() << " set their nick to: ";
	_clients[i].setNick(getNick(line));
	std::cout << _clients[i].getNick() << std::endl;
	checkRegistration(i);
}
