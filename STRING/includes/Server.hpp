#ifndef SERVER_HPP
# define SERVER_HPP

# include "Client.hpp"
# include "Channel.hpp"
# include "Colours.hpp"
# include "Defines.hpp"
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
# include <stdlib.h>

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
		std::map<int, Client> 	_clients;
		std::vector<Channel>	_channels;

		int				acceptClient();
		void			setPfds();
		bool			handleClientPoll(int i);
		void			processCommand(int i, std::string line);
		
		
		void	serverBroadcast(std::string str);
		void	channelBroadcast(std::string chName, std::string str);
		void	clientBroadcast(int i, std::string chName, std::string str);
		void	sendToClient(int i, std::string str);


		void	commandPass(int i, std::string line);
		void	commandNick(int i, std::string line);
		bool	isNickInUse(std::string toFind);
		void	commandUser(int i, std::string line);
		void	checkRegistration(int i);
		void	welcomeClient(int i);
		
		//*CHANNEL COMMANDS
		
		void	commandJoin(int i, std::string args);
		int		findOrCreateChannel(int i, std::string name);

		void	commandPart(int i, std::string name);
		void	commandKick(int i, std::string args);
		void	commandQuit(int i, std::string str);
		void	leaveChannel(int i, int chId);
		void	leaveAllChannels(int i);
		
		bool	isValidPrivmsg(std::string line);
		void	commandPrivmsg(int i, std::string line);

		void	commandMode(int i, std::string line);
		void 	executeCommandMode(int i, std::string chName, std::string opr, std::string args);
		void	modeInviteOnly(int i, int chId, bool inviteOnlyOrNot);
		void	modeTopicRestriction(int i, int chId, bool topicRestrict);
		void	modeKey(int i, int chId, std::string key, bool setKey);
		void	modeOp(int i, int chId, std::string user, bool opOrNot);
		void	modeLim(int i, int chId, std::string limitStr);
		
		void	commandInvite(int i, std::string name);
		bool 	findChannel(std::string nick, std::string chName);
		bool	hasInChannels(std::string name);
		
		void	commandTopic(int i, std::string line);
		void	noArgsTopic(int i, std::string chName);


		int			getClientId(std::string name);
		int			getChannelId(std::string name);
		std::string	getClientNick(int id);
		std::string	getChannelName(int id);
		bool		isUserInChannel(int i, int chId);

		
		void	exitServer();
		void	testClients(int i);
		void	testaux(int i);
		void	test();

	public:
		Server(char *port, char *pass);

		int			getSocket() { return (_socket); }
		int			getPort() { return (_port); }
		std::string getPass() { return (_pass); }
		void	testChannels();

		void	srvRun();
		
		
};

void	serverLog(std::string nick, std::string str);//remove

int		mySocket(int __domain, int __type, int __protocol);
void	myBind(int __fd, const sockaddr *__addr, socklen_t __len);
void	myListen(int __fd, int __n);
void	myPoll(pollfd *__fds, nfds_t __nfds, int __timeout);
size_t	myRecv(int __fd, char *__buf, size_t __n, int __flags);

#endif
