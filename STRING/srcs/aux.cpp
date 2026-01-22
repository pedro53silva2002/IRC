#include "../includes/Server.hpp"
int Channel::_globalChannelId;
int Client::_globalId;


/**
 * @brief Outputs a formatted server log message to the console.
 * 
 * Prints a message to standard output with a yellow "Server log:" prefix,
 * followed by the provided nickname and message string.
 * 
 * @param nick The nickname or context for the log entry.
 * @param str  The message to log.
 */
void	serverLog(std::string nick, std::string str)
{
	std::cout << YELLOW("Server log: ") << nick << " " << str << std::endl;
}

/**
 * @brief Updates the pollfd vector with all active client and server sockets.
 * 
 * Clears and repopulates the _pfds vector to include the server socket
 * and all connected client sockets for use with poll().
 * 
 * @note The server socket (_srvPfd) is always the first entry.
 * @note Each client's pollfd is added to the vector.
 */
void	Server::setPfds()
{
	_pfds.clear();
	_pfds.push_back(_srvPfd);
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
		_pfds.push_back(it->second.getPfd());
}

/**
 * @brief Sends a message to a specific client socket.
 * 
 * Formats the message with IRC line endings, logs the outgoing message,
 * and sends it to the client's socket.
 * 
 * @param i   The file descriptor index of the target client.
 * @param str The message to send.
 * 
 * @note Appends "\r\n" to the message before sending.
 * @note Logs the message using serverLog() before sending.
 */
void	Server::sendToClient(int i, std::string str) {
	std::string reply = str + "\r\n";
	serverLog(_clients[i].getNick(), "received string: [" + str + "]");
	send(_clients[i].getSocket(), reply.c_str(), reply.size(), 0);
}

/**
 * @brief Sends a message to all connected clients.
 * 
 * Iterates through all clients and sends the specified message to each one.
 * 
 * @param str The message to broadcast to all clients.
 * 
 * @note Uses sendToClient() for each client.
 */
void	Server::serverBroadcast(std::string str)
{
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++)	{
		sendToClient(it->first, str);
	}
}

 /**
 * @brief Sends a message to all clients in a specific channel.
 * 
 * Iterates through all clients in the specified channel and sends the given message.
 * 
 * @param chId The ID of the channel.
 * @param str  The message to send to all clients in the channel.
 */
void	Server::channelBroadcast(int chId, std::string str)
{
	for (std::vector<int>::iterator it = _channels[chId].getClientsInChannel().begin(); 
		it != _channels[chId].getClientsInChannel().end(); it++) {
			sendToClient(*it, str);
		}
}

/**
 * @brief Sends a message to all clients in a channel except the sender.
 * 
 * Iterates through all clients in the specified channel and sends the given message
 * to each client except the one identified by the provided index.
 * 
 * @param i    The file descriptor index of the sender client.
 * @param chId The ID of the channel.
 * @param str  The message to send to other clients in the channel.
 */
void	Server::clientBroadcast(int i, int chId, std::string str)
{
	for (std::vector<int>::iterator it = _channels[chId].getClientsInChannel().begin(); 
		it != _channels[chId].getClientsInChannel().end(); it++) {
			if (i != *it)
				sendToClient(*it, str);
	}
}

/**
 * @brief Retrieves the client ID (index) for a given nickname.
 * 
 * Searches through the list of clients and returns the index of the client
 * whose nickname matches the provided name.
 * 
 * @param name The nickname to search for.
 * 
 * @return The index of the client if found, or -1 if not found.
 * 
 * @note Starts searching from index 1 (skips 0).
 */
int Server::getClientId(std::string name)
{
	for (size_t i = 1; i < _clients.size(); i++)
	{
		if (name == _clients[i].getNick())
			return (i);
	}
	return (-1);
}

/**
 * @brief Retrieves the channel ID for a given channel name.
 * 
 * Searches through the list of channels and returns the ID of the channel
 * whose name matches the provided name.
 * 
 * @param name The name of the channel to search for.
 * 
 * @return The ID of the channel if found, or -1 if not found.
 */
int Server::getChannelId(std::string name)
{
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (name == _channels[i].getName())
			return (_channels[i].getId());
	}
	return (-1);
}

/**
 * @brief Checks if a client is a member of a specific channel.
 * 
 * Determines whether the client with the given index is present in the
 * specified channel's client list.
 * 
 * @param i    The file descriptor index of the client.
 * @param chId The ID of the channel.
 * 
 * @return true if the client is in the channel, false otherwise.
 * 
 * @note Returns false if the channel ID is -1.
 */
bool Server::isUserInChannel(int i, int chId)
{
	if (chId == -1)
		return (false);
	for (std::vector<int>::iterator it = _channels[chId].getClientsInChannel().begin(); 
		it != _channels[chId].getClientsInChannel().end(); it++) {
			if (i == *it)
				return (true);
	}
	return (false);
}

 /**
 * @brief Checks if a string consists only of numeric characters.
 * 
 * Iterates through the string to verify that each character is a digit.
 * 
 * @param str The string to check.
 * 
 * @return true if the string is numeric, false otherwise.
 */
bool	isNum(std::string str)
{
	std::string::iterator it = str.begin();
	while (it != str.end() && std::isdigit(*it))
		it++;
	if(it == str.end() && !str.empty())
		return (true);
	return (false);
}

/**
 * @brief Parses and validates command-line arguments for the server.
 * 
 * Ensures that the correct number of arguments are provided,
 * that the port is numeric and within valid range, and that
 * a non-empty password is given.
 * 
 * @param ac The argument count.
 * @param av The argument vector.
 * 
 * @return true if arguments are valid, false otherwise.
 * 
 * @note Expects exactly 2 arguments: <port> <password>.
 * @note Port must be an integer between 1024 and 65535.
 * @note Password must be a non-empty string.
 */
bool	parseMain(int ac, char **av)
{
	if (ac != 3 || !isNum(av[1])) {
		std::cout << RED("./ircserv <port> <password>") << std::endl;
		return (false);
	}
	if (atoi(av[1]) <= 1023 || atoi(av[1]) > 65535) {
		std::cout << RED("invalid port number");
		return (false);
	}
	if (strlen(av[2]) == 0) {
		std::cout << RED("needs password");
		return (false);
	}
	return (true);
}