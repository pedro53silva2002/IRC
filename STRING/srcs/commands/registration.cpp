#include "../includes/Server.hpp"
//todo Nick can be called after, has to change prefix

//according to other irc, is PASSWDMISMATCH, disconnected the client
//also according to other irc, PASS needs to come before NICK or USER
/**
 * @brief Handles the PASS command for client authentication.
 * 
 * Validates the password provided by the client against the server password.
 * If successful, marks the client as authenticated. This command must be
 * sent before NICK and USER during the registration process.
 * 
 * @param i    The file descriptor index of the client in the _clients map.
 * @param line The password string provided by the client.
 * 
 * @note Sends ERR_ALREADYREGISTERED (462) if the client is already authenticated.
 * @note Sends ERR_PASSWDMISMATCH (464) if the password is incorrect.
 */
void	Server::commandPass(int i, std::string line)
{
	if (_clients[i].isAuthenticated())
		return (sendToClient(i, ERR_ALREADYREGISTERED(_clients[i].getNick())));
	if (line.compare(_pass))
		return (sendToClient(i, ERR_PASSWDMISMATCH(_clients[i].getNick())));
	_clients[i].setAuthenticated(true);
	serverLog(_clients[i].getNick(), "has authenticated");
}

/**
 * @brief Validates the USER command arguments before execution.
 * 
 * Performs validation to ensure the client is authenticated, arguments
 * were provided, and the user hasn't already registered. Also validates
 * the USER command format: "<username> 0 * <realname>".
 * 
 * @param i    The file descriptor index of the client in the _clients map.
 * @param args The raw arguments: "<username> <mode> <unused> :<realname>".
 * 
 * @return true if validation passes.
 * @return false if validation fails (error sent to client).
 * 
 * @note Sends ERR_NOTREGISTERED (451) if client is not authenticated (no PASS).
 * @note Sends ERR_NEEDMOREPARAMS (461) if no arguments provided.
 * @note Sends ERR_ALREADYREGISTERED (462) if user already has username/realname set.
 * 
 */
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

/**
 * @brief Handles the USER command to set username and realname.
 * 
 * Parses the USER command arguments to extract and set the client's
 * username and realname. Part of the registration process along with
 * PASS and NICK commands.
 * 
 * @param i    The file descriptor index of the client in the _clients map.
 * @param args The raw arguments: "<username> <mode> <unused> :<realname>".
 * 
 * @note Logs the username and realname to stdout upon success.
 * @note Triggers registration check after setting user info.
 * 
 * @see isValidUser() for validation (authentication, params, already registered).
 * @see checkRegistration() for completing the registration process.
 */
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
		
	std::cout << _clients[i].getNick() << " set their username to: ";
	std::cout << _clients[i].getUsername() << " || " << _clients[i].getRealname() << std::endl;
	checkRegistration(i);
}

/**
 * @brief Validates the NICK command arguments before execution.
 * 
 * Performs validation to ensure the client is authenticated, a nickname
 * was provided, the nickname format is valid (no spaces, doesn't start
 * with ':', '#', or channel prefixes), and it's not already in use.
 * 
 * @param i    The file descriptor index of the client in the _clients map.
 * @param args The desired nickname.
 * 
 * @return true if validation passes.
 * @return false if validation fails (error sent to client).
 * 
 * @note Sends ERR_NOTREGISTERED (451) if client is not authenticated.
 * @note Sends ERR_NONICKNAMEGIVEN (431) if no nickname provided.
 * @note Sends ERR_ERRONEUSNICKNAME (432) if nickname contains invalid characters.
 * @note Sends ERR_NICKNAMEINUSE (433) if nickname is already taken.
 * 
 * @see isNickInUse() for checking nickname availability.
 */
bool	Server::isValidNick(int i,std::string args)
{
	if (!_clients[i].isAuthenticated())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);	
	if (args.empty())
		return (sendToClient(i, ERR_NONICKNAMEGIVEN()), false);
	if (args.find(' ') != std::string::npos || args[0] == ':' || args[0] == '#' || args.compare(0, 2, "#&") == 0 || args.compare(0, 2, "&#") == 0)
		return (sendToClient(i, ERR_ERRONEUSNICKNAME(_clients[i].getNick(), args)), false);
	if (isNickInUse(args))
		return (sendToClient(i, ERR_NICKNAMEINUSE(_clients[i].getNick(), args)), false);
	return (true);
}

/**
 * @brief Checks if a nickname is already in use by another client.
 * 
 * Iterates through all connected clients to determine if the given
 * nickname is already taken.
 * 
 * @param toFind The nickname to search for.
 * 
 * @return true if the nickname is already in use.
 * @return false if the nickname is available.
 */
bool	Server::isNickInUse(std::string toFind)
{
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++)	{
		if (_clients[it->first].getNick() == toFind)
			return (true);
	}
	return (false);
}

/**
 * @brief Handles the NICK command to set or change the client's nickname.
 * 
 * Sets the client's nickname after validation. Part of the registration
 * process along with PASS and USER commands. Can also be used after
 * registration to change nickname.
 * 
 * @param i    The file descriptor index of the client in the _clients map.
 * @param args The desired nickname.
 * 
 * @note Logs the nickname change to stdout.
 * @note Triggers registration check after setting nickname.
 * 
 * @see isValidNick() for validation (authentication, format, availability).
 * @see checkRegistration() for completing the registration process.
 * 
 */
void	Server::commandNick(int i, std::string args)
{
	//todo PUT THIS IN isValidNick
	//todo PUT THIS IN isValidNick
	if (!isValidNick(i, args))
		return ;
	
	std::cout << _clients[i].getNick() << " set their nick to: ";//no need for this, can just set it and use it after
	_clients[i].setNick(args);
	std::cout << _clients[i].getNick() << std::endl;

	//just outputs to client that NICK has been set
	//todo setPrefix();
	checkRegistration(i);
}

/**
 * @brief Sends welcome messages to a newly registered client.
 * 
 * Called after successful registration to send the standard IRC
 * welcome sequence to the client.
 * 
 * @param i The file descriptor index of the client in the _clients map.
 * 
 * @note Called by checkRegistration() upon successful registration.
 * 
 */
void	Server::welcomeClient(int i)
{
	sendToClient(i, "CAP * LS");
	// sendToClient(i, RPL_WELCOME(_clients[i].getNick(), _name));
	// sendToClient(i, RPL_YOURHOST(_name));
	// sendToClient(i, RPL_MYINFO(_name, _clients[i].getNick()));
	// sendToClient(i, RPL_MOTDSTART(_clients[i].getNick(), _name));
	// sendToClient(i, RPL_MOTD(_clients[i].getNick(), _motd));
	// sendToClient(i, RPL_ENDOFMOTD(_clients[i].getNick()));
}

/**
 * @brief Checks if a client has completed registration and finalizes it.
 * 
 * Verifies that the client has provided all required information (nick,
 * username, realname) and is not already registered. If all conditions
 * are met, marks the client as registered, sends welcome messages, and
 * sets the client's prefix.
 * 
 * @param i The file descriptor index of the client in the _clients map.
 * 
 * @note Registration requires: nick (not "*"), username, and realname.
 * @note Called by commandNick() and commandUser() after setting values.
 * 
 * @see welcomeClient() for sending the welcome message sequence.
 */
void	Server::checkRegistration(int i)
{
	if (!_clients[i].isRegistered() && !_clients[i].getNick().empty() && !_clients[i].getUsername().empty() 
		&& !_clients[i].getRealname().empty() && _clients[i].getNick() != "*")
		{
			_clients[i].setRegistered(true);
			welcomeClient(i);
			_clients[i].setPrefix();
		}
}
