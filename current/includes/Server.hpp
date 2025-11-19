#ifndef SERVER_HPP
# define SERVER_HPP
//divide these headers into each header
# include <iostream>
# include <sys/types.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <string.h>
# include <string>
# include <vector>
# include <poll.h>
# include <stdlib.h>//atoi
# include "Client.hpp"
# include "Colours.hpp"
# include "Defines.hpp"
# include "Channel.hpp"

class Server
{
	private:
		std::string		_name;
		int				_port;
		std::string		_pass;
		int				_socket;
		sockaddr_in		server_addr;
		pollfd			_srvPfd;

		std::vector<pollfd>		_pfds;
		std::vector<Client> 	_clients;
		std::vector<Channel>	_channels;

		int		acceptClient();
		void	setPfds();
		bool	handleClientPoll(int i);

		//AuthReg
		void	registration(int i);
		void	tryAuthClient(int i);
		void	tryPass(int i, char *bufPass);
		void	checkRegistration(int i);
		void	registerUser(int i);
		void	registerNick(int i);
		void	welcomeClient(int i);
		
		//Commands
		void	processCommand(int i);
		// int		getStatus(int i);
		void	commandQuit(int i, std::string str);

		//join
		void	commandJoin(int i, std::string name);
		int		findOrCreateChannel(int i, std::string name);
		//kick
		void	commandKick(int i, std::string toKick);
		//invite
		void	commandInvite(int i, std::string name);


		//send to channels
		void	sendToClientsInChannel(int i, std::string str);

		//utils
		int findChannel(Client client, std::vector<Channel> channels,std::string name);
		Client* foundInUsers(std::string name);

		//command from Angel: 		PRIVMSG Wiz :Hello are you receiving this message ?
		//sendtoClient to Wiz:		:Angel PRIVMSG Wiz :Hello are you receiving this message ?
		//command from dan:			PRIVMSG #coolpeople :Hi everyone!
		//sendtoClientinChannel:	:dan!~h@localhost PRIVMSG #coolpeople :Hi everyone!
		void	sendToClient(int id, std::string sender, std::string str);
		void	sendToClient(int i, std::string str);
		

		//just for testing
		void	exitServer();
		void	debugMessage(int i);
		void	testClients();

	public:
		//*CONSTRUCTORS
		Server(char *port, char *pass);

		//*GETTERS
		int			getSocket() { return (_socket); }
		int			getPort() { return (_port); }
		std::string getPass() { return (_pass); }


		void	srvRun();
		
		
};


void	serverLog(std::string nick, std::string str);

//*myFunctions
int		mySocket(int __domain, int __type, int __protocol);
void	myBind(int __fd, const sockaddr *__addr, socklen_t __len);
void	myListen(int __fd, int __n);
void	myPoll(pollfd *__fds, nfds_t __nfds, int __timeout);
size_t	myRecv(int __fd, char *__buf, size_t __n, int __flags);

#endif
