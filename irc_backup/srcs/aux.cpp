#include "../includes/Server.hpp"

void	serverLog(std::string nick, std::string str)
{
	std::cout << YELLOW("Server log: ") << nick << " " << str << std::endl;
}