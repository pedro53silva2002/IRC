#ifndef SERVER_HPP
# define SERVER_HPP

# include "Client.hpp"
# include "Channel.hpp"
# include "Defines.hpp"
# include "Utils.hpp"

const std::string SERVERNAME = "MyIRC";

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

		int		acceptClient();
		void	setPfds();
		bool	handleClientPoll(int);
		void	processCommand(int, std::string);
		
		
		void	serverBroadcast(std::string);
		void	channelBroadcast(int, std::string);
		void	clientBroadcast(int, int, std::string);
		void	sendToClient(int, std::string);
		
		//*COMMANDS
		void	commandPass(int, std::string);
		void	commandNick(int, std::string);
		bool	isValidNick(int, std::string);
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
		void	commandQuit(int, std::string);
		void	leaveChannel(int, int);
		
		void	commandPrivmsg(int, std::string);
		bool	isValidPrivmsg(int, std::string);
		void	privmsgChannel(int, std::string, std::string);
		void	privmsgUser(int, std::string, std::string);
		
		void	commandMode(int, std::string);
		bool	isValidMode(int, std::string);
		void	modeInviteOnly(int, int, bool *);
		void	modeTopicRestriction(int, int, bool *);
		void	modeKey(int, int, std::vector<std::string>, bool *, size_t *);
		void	modeOp(int, int, std::vector<std::string>, bool *, size_t *);
		void	modeLim(int, int, std::vector<std::string>, bool *, size_t *);
	
		void	commandInvite(int, std::string);

		void	commandTopic(int, std::string);
		bool	isValidTopic(int, std::string);


		int		getClientId(std::string);
		int		getChannelId(std::string);
		bool	isUserInChannel(int, int);

		void	test();

	public:
		//*CONSTRUCTORS
		Server();
		Server(char *port, char *pass);
		Server(const Server& other);
		Server& operator=(const Server& other);
		~Server();

		//*OTHERS
		void	srvRun();
};

#endif
