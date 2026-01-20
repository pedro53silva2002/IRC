#include "../includes/Server.hpp"
#include "../includes/Client.hpp"

//both the Clients and the Channels should be new, so i can save info to them before pushing them back

int		main(int ac, char **av)
{
	//todo parse
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
