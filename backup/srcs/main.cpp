#include "../includes/Server.hpp"
#include "../includes/Client.hpp"


int		main(int ac, char **av)
{
	if (ac != 3) {
		std::cout << RED("Bad arguments") << std::endl;
		return 1;
	}
	try {
		Server srv(av[1], av[2]);
		srv.srvRun();
	} catch (std::exception &e) {
		std::cerr << RED("Exception caught: ") << e.what() << std::endl;
	} catch (int n) {
		std::cerr << GREEN("Server exit ok: ") << n << std::endl;
	}

	return 0;
}


/** @brief Initiates the process of spliting the string into an array of words
 * 	@param result - array of words
 * 	@param str - string
 * 	@param c - separator
 *  @return Returns an array of words
*/

/*
	Client needs to register itself in server before using stuff like JOIN or PRIVMSG
	client should send info:
		recommended order by ircdocs:
			CAP LS 302 (will i be able to ignore this??)
			PASS
			NICK and USER
			CAP END

	server MUST send:
		RPL_WELCOME
		RPL_YOURHOST


	cases client should disconnect:
		client tries to talk before setting nick or user
		client send data with no newline or too long
*/
