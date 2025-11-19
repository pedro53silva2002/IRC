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
# include <vector>
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

		int			_channelId;//check if this is good
		std::string	_channelName;//check if this is good

		bool		_isOp;
		
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
			// _capped = false;
			_username = "";
			_nick = "*";
			_realname = "";
			_channelId = -1;
			_channelName = "";
			_isOp = false;
		}

		Client() {
			_id = -1;
			_socket = 0;
			_channelId = -1;
			_channelName = "";
			_isOp = false;
		}//DUMMY

		//*GETTERS
		int			getId() { return (_id); }
		int			getSocket() { return (_socket); }
		pollfd		&getPfd() { return (_pfd); }//why &
		bool		isAuthenticated() { return (_authenticated); }
		bool		isRegistered() { return (_registered); }
		// bool		isCapped() { return (_capped); }
		std::string	getUsername() { return (_username); }
		std::string	getNick() { return (_nick); }
		std::string	getRealname() { return (_realname); }
		bool		getOp() { return (_isOp); }
		int			getChannelId() { return (_channelId); }
		std::string	getChannelName() { return (_channelName); }

		//*SETTERS
		void	setAuthenticated(bool auth) { _authenticated = auth; }
		void	setRegistered(bool auth) { _registered = auth; }
		// void	setCapped(bool auth) { _capped = auth; }
		void	setUsername(std::string username) { _username = username; }
		void	setNick(std::string nick) { _nick = nick; }
		void	setRealname(std::string realname) { _realname = realname; }
		void	setChannelId(int channelId) { _channelId = channelId; }
		void	setchannelName(std::string channelName){ 
			if (_channelName == "")
			{
				channelName.erase(std::remove(channelName.begin(),channelName.end(), '\n'),channelName.end());
				_channelName = channelName;
			}
			else
				_channelName += ";" + channelName; 
		}
		void	setOp(bool op) { _isOp = op; }





};

#endif
