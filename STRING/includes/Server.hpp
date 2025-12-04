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

		int				acceptClient();
		std::string		setPrefixTemp(int i);
		void			setPfds();
		bool			handleClientPoll(int i);
		

		
		//*Commands
		void	processCommand(int i, std::string line);
		
		//*Registration commands, mostly done, needs more parsing still
		void	commandPass(int i, std::string line);
		void	commandUser(int i, std::string line);
		void	commandNick(int i, std::string line);
		void	checkRegistration(int i);
		void	welcomeClient(int i);
		
		
		void	commandJoin(int i, std::string name);
		int		findOrCreateChannel(int i, std::string name);

		void	commandKick(int i, std::string toKick);

		void	commandInvite(int i, std::string name);
		void	commandMode(int i, std::string line);
		void	commandQuit(int i, std::string str);//disconnect client


		void	sendToClientsInChannel(int i, std::string str);
		void	sendToClient(int id, std::string sender, std::string str);
		void	sendToClient(int i, std::string str);


		//utils
		int		findChannel(Client client, std::vector<Channel> channels,std::string name, std::string userToInvite, std::string channelToGet);
		Client*	foundInUsers(std::string name);
		void 	executeCommandMode(int i, std::string channelTarget, std::string opr, std::string user);
		

		//just for testing
		void	exitServer();
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

		//command from Angel: 		PRIVMSG Wiz :Hello are you receiving this message ?
		//sendtoClient to Wiz:		:Angel PRIVMSG Wiz :Hello are you receiving this message ?
		//command from dan:			PRIVMSG #coolpeople :Hi everyone!
		//sendtoClientinChannel:	:dan!~h@localhost PRIVMSG #coolpeople :Hi everyone!

void	serverLog(std::string nick, std::string str);

//*myFunctions
int		mySocket(int __domain, int __type, int __protocol);
void	myBind(int __fd, const sockaddr *__addr, socklen_t __len);
void	myListen(int __fd, int __n);
void	myPoll(pollfd *__fds, nfds_t __nfds, int __timeout);
size_t	myRecv(int __fd, char *__buf, size_t __n, int __flags);

#endif
