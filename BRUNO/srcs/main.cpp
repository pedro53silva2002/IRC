#include "../includes/IRC.hpp"

/* 
	Client needs to put correct pass to be able to talk, 
	otherwise it gets disconnected and socket closed
*/

class Server
{
	private:
		int				_port;
		char			_pass;
		int				_socket;
		sockaddr_in		server_addr;
	public:
		Server(char *port, char *pass)
		{
			_port = atoi(port);
			_socket = socket(AF_INET, SOCK_STREAM, 0);
			if (_socket == -1)
				throw (std::runtime_error("Cant create socket"));

			int opt = 1;
			setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
			server_addr.sin_family = AF_INET;
			server_addr.sin_port = htons(_port);
			inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr);

			if (bind(_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == -1)
				throw (std::runtime_error("Cant bind to IP/port"));

			if (listen(_socket, SOMAXCONN) == -1)
				throw (std::runtime_error("Cant listen"));
			
			std::cout << "Server open in port: " << _port << std::endl;
		}

		int getSocket() {
			return (_socket);
		}
};

class Client
{
	private:
		static int	_globalId;
		int			_id;
		int			_socket;
		pollfd		_pfd;
	public:
		Client(int srvSocket)
		{
			_id = _globalId++;
			_socket = srvSocket;
			_pfd.fd = _socket;
			_pfd.events = POLLIN;
			_pfd.revents = 0;
		}

		int		getId() {
			return (_id);
		}
		int		getSocket() {
			return (_socket);
		}
		pollfd	&getPfd() {
			return (_pfd);
		}
};

//so weird so random
int Client::_globalId;

bool	shouldServerExit(Server srv, std::vector<Client> clients, char buf[])
{
	if (strncmp(buf, "exit", 4) == 0) {
		std::cout << "exiting server\n";
		for (std::vector<Client>::iterator client_it = clients.begin(); client_it != clients.end(); client_it++)
			close(client_it->getSocket());
		close(srv.getSocket());
		return (true);
	}
	return (false);
}

int		acceptClient(int srvSocket)
{
	int			socket;
	sockaddr_in	client;
	socklen_t	clientSize = sizeof(client);
	char		host[NI_MAXHOST];
	
	socket = accept(srvSocket, (sockaddr*)&client, &clientSize);
	if (socket == -1)
		throw (std::runtime_error("Problem with client connecting"));
	memset(host, 0, NI_MAXHOST);
	
	inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
	std::cout << host << " manually connected on " << ntohs(client.sin_port) << std::endl;
	return (socket);
}

void	IRC(char *port, char *pass)
{
	Server srv(port, pass);//it will take parameters after, for now its hard coded

	pollfd srv_pfd;
	srv_pfd.fd = srv.getSocket();
	srv_pfd.events = POLLIN;
	srv_pfd.revents = 0;
	std::vector<pollfd> pfds;
	
	std::vector<Client> clients;
	
	while (1)
	{
		//save the data
		pfds.clear();
		pfds.push_back(srv_pfd);
		for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
			pfds.push_back(it->getPfd());

		if (poll(pfds.data(), pfds.size(), -1) == -1)
			throw (std::runtime_error("Poll failed"));
		if (pfds[0].revents & POLLIN)//* Client Connecting
		{
			int socket = acceptClient(srv.getSocket());
			clients.push_back(Client(socket));
		}
		for (int i = 1; i < pfds.size(); i++)//*loop through clients
		{
			if (pfds[i].revents & POLLIN)//*print message from client
			{	
				char buf[512];
				int bytesRecv = recv(pfds[i].fd, buf, sizeof(buf), 0);
				if (bytesRecv == -1)
					throw (std::runtime_error("There was a connection issue: 1"));
				if (bytesRecv == 0) {
					std::cout << "The client disconnected" << std::endl;
					close (pfds[i].fd);
					clients.erase(clients.begin() + (i - 1));
				} else {
					buf[bytesRecv] = 0;
					//*debugging line, it will use username instead of getId
					std::cout << "Client " << clients[i - 1].getId() + 1<< " said: " << buf;
					if (strcmp(buf, pass))
						std::cout << "Password correct\nClient should now be able to set user and talk\n";
					else {
						std::cout << "Password incorrect\nClient should close\n";
						// close (pfds[i].fd);
						// clients.erase(clients.begin() + (i - 1));
					}
					//! Now do password acceptance
					if (shouldServerExit(srv, clients, buf))//TEMPORARY
						return ;
				}
			}
		}
	}
	close(srv.getSocket());
}

int		main(int ac, char **av)
{
	if (ac != 3) {
		std::cout << "Bad arguments" << std::endl;
		return 1;
	}
	try {
		IRC(av[1], av[2]);
	} catch (std::exception &e) {
		std::cerr << "Exception caught! " << e.what() << std::endl;
	}

	return 0;
}



