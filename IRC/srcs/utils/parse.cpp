#include "../../includes/Server.hpp"

bool	isNum(std::string str)
{
	std::string::iterator it = str.begin();
	while (it != str.end() && std::isdigit(*it))
		it++;
	if(it == str.end() && !str.empty())
		return (true);
	return (false);
}

bool	parseMain(int ac, char **av)
{
	if (ac != 3 || !isNum(av[1])) {
		std::cout << RED("./ircserv <port> <password>") << std::endl;
		return (false);
	}
	if (atoi(av[1]) <= 1023 || atoi(av[1]) > 65535) {
		std::cout << RED("invalid port number");
		return (false);
	}
	if (strlen(av[2]) == 0) {
		std::cout << RED("needs password");
		return (false);
	}
	return (true);
}
