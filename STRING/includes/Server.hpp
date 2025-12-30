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
		void			processCommand(int i, std::string line);
		

		
		//only difference is it also sends to current user
		void	serverBroadcast(int i, std::string chName, std::string str);
		void	clientBroadcast(int i, std::string chName, std::string str);
		void	sendToClient(int i, std::string str);


		//*REGISTRATION
		//*pass
		void	commandPass(int i, std::string line);
		//*nick
		void	commandNick(int i, std::string line);
		bool	isNickInUse(std::string toFind);
		//*user
		void	commandUser(int i, std::string line);
		//*others
		void	checkRegistration(int i);
		void	welcomeClient(int i);
		
		//*CHANNEL COMMANDS
		//*privmsg
		bool	isValidPrivmsg(std::string line);
		void	commandPrivmsg(int i, std::string line);
		//*join
		void	commandJoin(int i, std::string args);
		//*part
		void	commandPart(int i, std::string name);
		//*kick
		void	commandKick(int i, std::string args);
		//*mode
		void	commandMode(int i, std::string line);
		void 	executeCommandMode(int i, std::string chName, std::string opr, std::string args);
		void	modeInviteOnly(int i,std::string channelTarget, bool inviteOnlyOrNot);
		void	modeTopicRestriction(int i,std::string channelTarget, bool topicRestrict);
		void	modeKey(int i,std::string channelTarget, std::string key, bool setKey);//setKey is to know if its +k or -k
		void	modeOp(int i,std::string channelTarget, std::string user, bool opOrNot);
		void	modeLim(int i,std::string channelTarget, std::string limitStr);
		//*invite
		void	commandInvite(int i, std::string name);
		//*topic
		void	commandTopic(int i, std::string line);
		void	noArgsTopic(int i, std::string chName);
		//*quit
		void	commandQuit(int i, std::string str);


		//utils, find out where to put these to be grouped
		int		findChannel(Client client, std::vector<Channel> channels,std::string name, std::string userToInvite, std::string channelToGet);
		Client*	foundInUsers(std::string name);
		int		findOrCreateChannel(int i, std::string name);
		bool	hasInChannels(std::string name);
		

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

//!THIS IS BAD TO BE HERE
bool hasInUserChannels(Client client, std::string name);
//!THIS IS BAD TO BE HERE

void	serverLog(std::string nick, std::string str);

//*myFunctions
int		mySocket(int __domain, int __type, int __protocol);
void	myBind(int __fd, const sockaddr *__addr, socklen_t __len);
void	myListen(int __fd, int __n);
void	myPoll(pollfd *__fds, nfds_t __nfds, int __timeout);
size_t	myRecv(int __fd, char *__buf, size_t __n, int __flags);

#endif
