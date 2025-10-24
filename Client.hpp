#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <deque>
#include <list>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <stdio.h>
#include <cmath>
#include <limits>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <map>
#include <queue>
#include <limits>
#include <exception>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>       // for getaddrinfo, freeaddrinfo, addrinfo
#include <netinet/in.h>  // for sockaddr_in, htons, INADDR_ANY
#include <arpa/inet.h>   // for inet_ntoa, inet_addr
#include <unistd.h>     // for close
#include <poll.h>     // for close

class Client
{
	private:
		struct sockaddr_in _clientAddr;
		socklen_t _clientLen;
		int _clientSocket;
		pollfd 		_pfd;
		char		_host[NI_MAXHOST];
		char		_svc[NI_MAXSERV];
	public:
		Client();
		Client(Client const & src);
		Client & operator=(Client const & src);
		~Client();
		struct sockaddr_in &getClientAddr();
		socklen_t &getClientLen();
		int getClientSocket() const;
		void setClientSocket(int clientSocket);
		char *getHost();
		char *getSvc();
		pollfd getPfd();
};

#endif