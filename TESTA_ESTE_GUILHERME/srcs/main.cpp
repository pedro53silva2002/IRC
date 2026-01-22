#include "../includes/Server.hpp"

Server *srv = NULL;

void signalHandler(int signum)
{
	if (signum == SIGPIPE || signum == SIGSEGV)
		return;
	if (srv)
		delete srv;
	exit(signum);
}

int		main(int ac, char **av)
{
	if (!parseMain(ac, av)) 
		return (1);

	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
	signal(SIGPIPE, signalHandler);
	try {
		srv = new Server(av[1], av[2]);
		srv->srvRun();
	} catch (std::exception &e) {
		std::cerr << RED("Exception caught: ") << e.what() << std::endl;
	} catch (int n) {
		std::cerr << GREEN("Server exit ok: ") << n << std::endl;
	}
	if (srv)
		delete (srv);
	

	return (0);
}
