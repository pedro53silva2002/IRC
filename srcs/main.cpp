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
	}

	return 0;
}

/* 
	Client needs to register itself in server before using stuff like JOIN or PRIVMSG
	client should send info:
		recommended order by ircdocs:
			CAP LS 302 (will i be able to ignore this??)
			PASS
			NICK and USER
			capability negotiation (what??)
			SASL (more what??)
			CAP END

	have server send a ping, and client send a pong

	server MUST send:
		RPL_WELCOME
		RPL_YOURHOST
		RPL_CREATED
		RPL_MYINFO
		at least RPL_ISUPPORT numeric to the client
		may send other numerics and messages
		Server should respond as if client sent LUSERS and return numerics
		Server should respond as if client sent MOTD
		if user has client modes, set them automatically

	(all this info idk what it means xD)


	cases client should disconnect:
		client tries to talk before setting nick or user
		client send data with no newline or too long
		sends QUIT command
		ping timeout
*/
