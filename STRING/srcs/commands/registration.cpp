#include "../includes/Server.hpp"

//according to other irc, is PASSWDMISMATCH, disconnected the client
//also according to other irc, PASS needs to come before NICK or USER
/**
 * @brief Handles the IRC PASS command for client authentication.
 * 
 * Validates the provided password against the server password and
 * marks the client as authenticated if successful.
 * 
 * @param i    The file descriptor index of the client sending the command.
 * @param line The password provided by the client.
 * 
 * @note Sends ERR_ALREADYREGISTERED (462) if the client is already authenticated.
 * @note Sends ERR_PASSWDMISMATCH (464) if the password is incorrect.
 * @note Logs successful authentication to the server log.
 * @note PASS must be sent before NICK or USER commands.
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
 * were provided, the user is not already registered, and the arguments
 * follow the expected format: "<username> 0 * <realname>".
 * 
 * @param i    The file descriptor index of the client sending the command.
 * @param args The raw arguments: "<username> <mode> <unused> :<realname>".
 * 
 * @return true if validation passes.
 * @return false if validation fails (error sent to client).
 * 
 * @note Sends ERR_NOTREGISTERED (451) if client is not authenticated.
 * @note Sends ERR_NEEDMOREPARAMS (461) if no arguments provided.
 * @note Sends ERR_ALREADYREGISTERED (462) if username/realname already set.
 * @note Expects at least 4 parameters with second param starting with '0' and third with '*'.
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
 * @brief Handles the IRC USER command for client registration.
 * 
 * Extracts and sets the username and real name for the client,
 * then checks if registration can be completed.
 * 
 * @param i    The file descriptor index of the client sending the command.
 * @param args The raw arguments: "<username> <mode> <unused> :<realname>".
 * 
 * @note Calls isValidUser() for validation before processing.
 * @note Sets the client's username and real name.
 * @note Calls checkRegistration() to complete registration if possible.
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
 * Performs validation to ensure the client is authenticated,
 * that a nickname was provided, and that the nickname is valid
 * and not already in use.
 * 
 * @param i    The file descriptor index of the client sending the command.
 * @param args The raw arguments: "<nickname>".
 * 
 * @return true if validation passes.
 * @return false if validation fails (error sent to client).
 * 
 * @note Sends ERR_NOTREGISTERED (451) if client is not authenticated.
 * @note Sends ERR_NONICKNAMEGIVEN (431) if no nickname provided.
 * @note Sends ERR_ERRONEUSNICKNAME (432) if nickname is invalid.
 * @note Sends ERR_NICKNAMEINUSE (433) if nickname is already taken.
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
 * @brief Checks if a nickname is already in use by any connected client.
 * 
 * Iterates through all connected clients to determine if the given
 * nickname is currently assigned to any of them.
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
 * @brief Handles the IRC NICK command to set or change a client's nickname.
 * 
 * Validates the provided nickname and, if valid, updates the client's
 * nickname and checks for registration completion.
 * 
 * @param i    The file descriptor index of the client sending the command.
 * @param args The raw arguments: "<nickname>".
 * 
 * @note Calls isValidNick() for validation before processing.
 * @note Updates the client's nickname upon successful validation.
 * @note Calls checkRegistration() to complete registration if possible.
 */
void	Server::commandNick(int i, std::string args)
{
	if (!isValidNick(i, args))
		return ;
	std::string oldNick = _clients[i].getNick();
	
	_clients[i].setNick(args);

	if (_clients[i].isRegistered())
		serverBroadcast(_clients[i].getPrefix() + " NICK " + args);
	checkRegistration(i);
	_clients[i].setPrefix();
}


/**
 * @brief Sends the welcome messages to a newly registered client.
 * 
 * Sends a series of standard IRC welcome messages to the client
 * upon successful registration, including server capabilities
 * and the message of the day (MOTD).
 * 
 * @param i The file descriptor index of the client to welcome.
 * 
 * @note Currently only sends "CAP * LS" as a placeholder.
 */
void	Server::welcomeClient(int i)
{
	sendToClient(i, "CAP * LS");
	sendToClient(i, RPL_WELCOME(_clients[i].getNick(), _name));
	sendToClient(i, RPL_YOURHOST(_name));
	sendToClient(i, RPL_MYINFO(_name, _clients[i].getNick()));
	sendToClient(i, RPL_MOTDSTART(_clients[i].getNick(), _name));
	sendToClient(i, RPL_MOTD(_clients[i].getNick(), _motd));
	sendToClient(i, RPL_ENDOFMOTD(_clients[i].getNick()));
}

/**
 * @brief Checks if a client has completed registration requirements.
 * 
 * Verifies that the client has set a nickname, username, and real name.
 * If all conditions are met, marks the client as registered and
 * sends the welcome messages.
 * 
 * @param i The file descriptor index of the client to check.
 * 
 * @note A client is considered registered if they have a non-empty
 *       nickname (not "*"), username, and real name.
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
