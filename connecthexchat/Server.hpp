#ifndef SERVER_HPP
#define SERVER_HPP

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

class Server
{
	private:
		std::string _port;
		std::string _password;
		sockaddr_in _serverAddr;
		struct addrinfo _hints;
		struct addrinfo *_res;
		int _ServerSocket;

	public:
        Server();
		Server(std::string port, std::string password);
		Server(Server const & src);
		Server & operator=(Server const & src);
		~Server();
		std::string getPort() const;
		std::string getPassword() const;
		int getServerSocket() const;
		const struct addrinfo &getHints() const;
		struct addrinfo *getRes() const;
		void setRes(struct addrinfo *res);
		void setServerSocket(int serverSocket);
};

#endif