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

class Server
{
	private:
		int				_port;
		std::string		_pass;

		int				_socket;
		sockaddr_in		server_addr;

		pollfd				_srvPfd;
		std::vector<pollfd> _pfds;
		std::vector<Client> _clients;

		int		acceptClient();
		void	setPfds();
		int		handleClientPoll(int i);
		void	disconnectClient(Client client, int i);

		//just for testing
		bool	shouldServerExit(char buf[]);

	public:
		//*CONSTRUCTORS
		Server(char *port, char *pass);

		//*GETTERS
		int			getSocket() { return (_socket); }
		int			getPort() { return (_port); }
		std::string getPass() { return (_pass); }


		void	srvRun();


		
};

//*helpers
int		mySocket(int __domain, int __type, int __protocol);
void	myBind(int __fd, const sockaddr *__addr, socklen_t __len);
void	myListen(int __fd, int __n);
void	myPoll(pollfd *__fds, nfds_t __nfds, int __timeout);
size_t	myRecv(int __fd, char *__buf, size_t __n, int __flags);

#endif


