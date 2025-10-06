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

class Server
{
	private:
		std::string _port;
		std::string _password;

	public:
        Server();
		Server(std::string port, std::string password);
		Server(Server const & src);
		Server & operator=(Server const & src);
		~Server();
		std::string getPort() const;
		std::string getPassword() const;
};

#endif