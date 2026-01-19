#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Headers.hpp"

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
		Client();
		Client(int srvSocket);
		Client(const Client& other);
		Client& operator=(const Client& other);

		int			getId();
		int			getSocket();
		pollfd		&getPfd();
		bool		isAuthenticated();
		bool		isRegistered();
		std::string	getPrefix();
		std::string	getHost();
		std::string	getUsername();
		std::string	getNick();
		std::string	getRealname();
		std::map<int, std::string>	&getChannels();

		void	setId(int id);
		void	setAuthenticated(bool auth);
		void	setRegistered(bool auth);
		void	setPrefix();
		void	setHost(std::string host);
		void	setUsername(std::string username);
		void	setNick(std::string nick);
		void	setRealname(std::string realname);
		void	setChannel(int chId, std::string chName);
};

#endif
