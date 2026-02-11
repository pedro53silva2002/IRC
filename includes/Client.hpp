#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Utils.hpp"

class Client
{
	private:
		static int	_globalId;
		int			_id;
		int			_socket;
		pollfd		_pfd;
		bool		_authenticated;
		bool		_registered;
		std::string	_username;
		std::string _nick;
		std::string	_realname;
		std::string	_host;
		std::string	_prefix;
		std::map<int, std::string> _chans;
		
	public:
		//*CONSTRUCTORS
		Client();
		Client(int srvSocket);
		Client(const Client& other);
		Client& operator=(const Client& other);
		~Client();

		//*GETTERS
		int			getId();
		int			getSocket();
		pollfd		&getPfd();
		bool		isAuthenticated();
		bool		isRegistered();
		std::string	getPrefix();
		std::string	getUsername();
		std::string	getNick();
		std::string	getRealname();
		std::map<int, std::string>	&getChannels();

		//*SETTERS
		void	setId(int);
		void	setAuthenticated(bool);
		void	setRegistered(bool);
		void	setPrefix();
		void	setHost(std::string);
		void	setUsername(std::string);
		void	setNick(std::string);
		void	setRealname(std::string);
		void	setChannel(int, std::string);
};

#endif
