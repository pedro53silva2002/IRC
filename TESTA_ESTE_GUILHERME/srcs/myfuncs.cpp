#include "../includes/Server.hpp"

int		mySocket(int __domain, int __type, int __protocol)
{
	int temp = socket(__domain, __type, __protocol);
	if (temp == -1)
		throw (std::runtime_error("Cant create socket"));
	return (temp);
}

void	myBind(int __fd, const sockaddr *__addr, socklen_t __len)
{
	if (bind(__fd, __addr, __len) == -1)
		throw (std::runtime_error("Cant bind to IP/port"));
}

void	myListen(int __fd, int __n)
{
	if (listen(__fd, __n) == -1)
		throw (std::runtime_error("Cant listen"));
}

void	myPoll(pollfd *__fds, nfds_t __nfds, int __timeout)
{
	if (poll(__fds, __nfds, __timeout) == -1)
		throw (std::runtime_error("Poll failed"));
}

size_t	myRecv(int __fd, char *__buf, size_t __n, int __flags)
{
	int temp = recv(__fd, __buf, __n, __flags);
	if (temp == -1)
		throw (std::runtime_error("There was a connection issue"));
	return (temp);
}