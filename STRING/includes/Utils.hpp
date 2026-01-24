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

#define RST "\033[1;0m"
#define DRED "\033[1;31m"
#define DGREEN "\033[1;32m"
#define DYELLOW "\033[1;33m"

#define RED(x) DRED x RST
#define GREEN(x) DGREEN x RST
#define YELLOW(x) DYELLOW x RST

#endif