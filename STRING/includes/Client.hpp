#ifndef CLIENT_HPP
# define CLIENT_HPP
//divide these headers into each header
# include <iostream>
# include <sys/types.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <string.h>
# include <string>
# include <vector>//maybe not needed
# include <map>
# include <poll.h>
# include <stdlib.h>//atoi
# include "Colours.hpp"
# include "Channel.hpp"
# include <algorithm>


//todo when setting username and nick, have the setUser and setNick do the parsing
class Client
{
	private:
		static int	_globalId;
		int			_id;

		int			_socket;
		pollfd		_pfd;

		//INFO
		bool		_authenticated;
		bool		_registered;
		// bool		_capped;
		std::string	_username;
		std::string _nick;
		std::string	_realname;

		std::map<int, std::string> _chans;

		std::string	_host;
		std::string	_prefix;
		
	public:
		//*CONSTRUCTORS
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
			_chans[0] = "";
			_host = "";
			_prefix = "";
		}

		Client() {
			_id = -1;
			_socket = 0;
			_chans[0] = "";
		}//DUMMY

		//*GETTERS
		int			getId() { return (_id); }
		int			getSocket() { return (_socket); }
		pollfd		&getPfd() { return (_pfd); }//why &
		bool		isAuthenticated() { return (_authenticated); }
		bool		isRegistered() { return (_registered); }
		std::string	getPrefix() { return (_prefix); }
		std::string	getHost() { return (_host); }

		std::string	getUsername() { return (_username); }
		std::string	getNick() { return (_nick); }
		std::string	getRealname() { return (_realname); }

		std::map<int, std::string>	getChannels() { return (_chans); }
		std::string	getChannelNameNew(int id) { return (_chans[id]); }//rename
		int			getChannelIdNew(std::string chName) {//rename
			for (std::map<int, std::string>::iterator it = _chans.begin(); it != _chans.end(); it++) {
				if (it->second == chName)
					return (it->first);
			}
			return (-1);//HOPEFULLY THIS WORKS
		}

		//*SETTERS
		void	setAuthenticated(bool auth) { _authenticated = auth; }
		void	setRegistered(bool auth) { _registered = auth; }
		void	setPrefix(std::string prefix) { _prefix = prefix; }
		void	setHost(std::string host) { _host = host; }

		void	setUsername(std::string username) { _username = username; }
		void	setNick(std::string nick) { _nick = nick; }
		void	setRealname(std::string realname) { _realname = realname; }

		void	setChannel(int id, std::string chName) {
			_chans.insert(std::make_pair(id, chName));
		}
		void	leaveChannel(int id) {
			
		}


};

#endif
