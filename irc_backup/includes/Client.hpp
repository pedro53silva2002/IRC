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
		bool		_capped;
		std::string	_username;
		std::string _nick;
		std::string	_realname;

		char		*_buf;

	public:
		id_t		_bytesRecv;//any other way?
		//*CONSTRUCTORS

		Client(int srvSocket) {
			_id = _globalId++;
			_socket = srvSocket;
			_pfd.fd = _socket;
			_pfd.events = POLLIN;
			_pfd.revents = 0;
			_authenticated = false;
			_registered = false;
			_capped = false;
			_username = "";
			_nick = "*";
			_realname = "";
		}

		Client() {
			_id = -1;
			_socket = 0;
		}//DUMMY

		//*GETTERS
		int		getId() { return (_id); }
		int		getSocket() { return (_socket); }
		pollfd	&getPfd() { return (_pfd); }//why &

		bool isCapped() {
			return (_capped);
		}
		void setCapped(bool auth) {
			_capped = auth;
		}
		bool isAuthenticated() {
			return (_authenticated);
		}
		void setAuthenticated(bool auth) {
			_authenticated = auth;
		}
		bool isRegistered() {
			return (_registered);
		}
		void setRegistered(bool auth) {
			_registered = auth;
		}
		std::string	getUsername() {
			return (_username);
		}
		void		setUsername(std::string username) {
			_username = username;
		}
		//check order
		std::string	getNick() {
			return (_nick);
		}
		void		setNick(std::string nick) {
			_nick = nick;
		}
		std::string	getRealname() {
			return (_realname);
		}
		void	setRealname(std::string realname) {
			_realname = realname;
		}


		void	setBuf(char buf[]) {
			_buf = buf;
		}
		char*	getBuf() {
			return (_buf);
		}



};


#endif

