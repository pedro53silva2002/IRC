#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Colours.hpp"
# include <iostream>
# include <sys/types.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <string.h>
# include <string>
# include <vector>
# include <map>
# include <poll.h>
# include <stdlib.h>
# include <algorithm>


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

		std::map<int, std::string> _chans;
/* 
				char arr[] = {0 1 2 3 4 5}; 
				char* arr[] = {{"bruno", 22} "luis", isaac}; 
				arr[1] == world

				map = {(22, "bruno")(42, "ola")(68, "bruno")}
				map[3] == [""]
				map[22] == bruno
				map[42] == ola


*/

		std::string	_host;
		std::string	_prefix;
		
	public:
		Client(int srvSocket) {
			_id = _globalId++;
			_socket = srvSocket;
			_pfd.fd = _socket;
			_pfd.events = POLLIN;
			_pfd.revents = 0;
			_authenticated = false;
			_registered = false;
			_username = "";
			_nick = "*";
			_realname = "";
			_host = "";
			_prefix = "";
		}

		Client() {
			_id = -1;
		}

		int			getId() { return (_id); }
		int			getSocket() { return (_socket); }
		pollfd		&getPfd() { return (_pfd); }
		bool		isAuthenticated() { return (_authenticated); }
		bool		isRegistered() { return (_registered); }
		std::string	getPrefix() { return (_prefix); }
		std::string	getHost() { return (_host); }
		std::string	getUsername() { return (_username); }
		std::string	getNick() { return (_nick); }
		std::string	getRealname() { return (_realname); }
		std::map<int, std::string>	&getChannels() { return (_chans); }

		void	setId(int id) { _id = id; }
		void	setAuthenticated(bool auth) { _authenticated = auth; }
		void	setRegistered(bool auth) { _registered = auth; }
		void	setPrefix() { _prefix = ":" + _nick + "!" + _username + "@" + _host; }
		void	setHost(std::string host) { _host = host; }
		void	setUsername(std::string username) { _username = username; }
		void	setNick(std::string nick) { _nick = nick; }
		void	setRealname(std::string realname) { _realname = realname; }
		void	setChannel(int chId, std::string chName) { _chans.insert(std::make_pair(chId, chName)); }
};

#endif
