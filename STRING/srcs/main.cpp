#include "../includes/Server.hpp"

//both the Clients and the Channels should be new, so i can save info to them before pushing them back

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
	//todo parse
	if (!parseMain(ac, av)) 
		return 1;

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
	

	return 0;
}
