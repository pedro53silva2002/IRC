#ifndef SERVER_HPP
# define SERVER_HPP

# include "Client.hpp"
# include "Channel.hpp"
# include "Defines.hpp"
# include "Utils.hpp"

class Server
{
	private:
		std::string		_name;
		int				_port;
		std::string		_pass;
		int				_socket;
		sockaddr_in		server_addr;
		pollfd			_srvPfd;
		std::string		_motd;

		std::vector<pollfd>		_pfds;
		std::map<int, Client> 	_clients;
		std::vector<Channel>	_channels;

		int				acceptClient();
		void			setPfds();
		bool			handleClientPoll(int i);
		void			processCommand(int i, std::string line);
		
		
		void	serverBroadcast(std::string str);
		void	channelBroadcast(int chId, std::string str);
		void	clientBroadcast(int i, int chId, std::string str);
		void	sendToClient(int i, std::string str);
		
		//*COMMANDS
		void	commandPass(int, std::string);
		void	commandNick(int, std::string);
		bool	isValidNick(int, std::string);
		bool	isNickInUse(std::string);
		void	commandUser(int, std::string);
		bool	isValidUser(int, std::string);
		void	checkRegistration(int);
		void	welcomeClient(int);

		void	commandJoin(int, std::string);
		bool	isValidJoin(int, std::string);
		int		findOrCreateChannel(int, std::string);
		
		void	commandPart(int, std::string);
		bool	isValidPart(int, std::string);
		void	commandKick(int, std::string);
		bool	isValidKick(int, std::string);
		void	commandQuit(int, std::string);
		void	leaveChannel(int, int);
		
		void	commandPrivmsg(int, std::string);
		bool	isValidPrivmsg(int, std::string);
		
		void	commandMode(int, std::string);
		bool	isValidMode(int, std::string);
		void 	executeCommandMode(int, std::string, std::string, std::string);
		void	outputMode(int, int, bool, char, std::string);
		void	modeInviteOnly(int, int, bool);
		void	modeTopicRestriction(int, int, bool);
		void	modeKey(int, int, std::string, bool);
		void	modeOp(int, int, std::string, bool);
		void	modeLim(int, int, std::string);
		
		void	commandInvite(int, std::string);
		bool	isValidInvite(int, std::string);

		void	commandTopic(int, std::string);
		bool	isValidTopic(int, std::string);
		void	noArgsTopic(int, std::string);



		int			getClientId(std::string name);
		int			getChannelId(std::string name);
		bool		isUserInChannel(int i, int chId);

		void	test();

	public:
		Server();
		Server(char *port, char *pass);
		Server(const Server& other);
		Server& operator=(const Server& other);
		~Server();

		void	srvRun();	
};

void	serverLog(std::string nick, std::string str);

int		mySocket(int __domain, int __type, int __protocol);
void	myBind(int __fd, const sockaddr *__addr, socklen_t __len);
void	myListen(int __fd, int __n);
void	myPoll(pollfd *__fds, nfds_t __nfds, int __timeout);
size_t	myRecv(int __fd, char *__buf, size_t __n, int __flags);

bool	parseMain(int ac, char **av);
bool	isNum(std::string str);

#endif
