#ifndef UTILS_HPP
# define UTILS_HPP

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
# include <stdlib.h>
# include <algorithm>
# include <map>
# include <signal.h>
# include <sstream>
# include <ctime>

# define RST "\033[1;0m"
# define DRED "\033[1;31m"
# define DGREEN "\033[1;32m"
# define DYELLOW "\033[1;33m"

# define RED(x) DRED x RST
# define GREEN(x) DGREEN x RST
# define YELLOW(x) DYELLOW x RST

int		mySocket(int __domain, int __type, int __protocol);
void	myBind(int __fd, const sockaddr *__addr, socklen_t __len);
void	myListen(int __fd, int __n);
void	myPoll(pollfd *__fds, nfds_t __nfds, int __timeout);
size_t	myRecv(int __fd, char *__buf, size_t __n, int __flags);
void	serverLog(std::string nick, std::string str);
bool	parseMain(int ac, char **av);
bool	isNum(std::string str);
std::vector<std::string> getArgs(std::string line);

#endif