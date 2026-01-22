#include "../includes/Server.hpp"

/**
 * @brief Creates a new socket and checks for errors.
 * 
 * Wrapper around the socket() system call that throws a runtime error
 * if socket creation fails.
 * 
 * @param __domain   The communication domain (e.g., AF_INET).
 * @param __type     The socket type (e.g., SOCK_STREAM).
 * @param __protocol The protocol to be used.
 * 
 * @return The file descriptor for the new socket.
 * 
 * @throws std::runtime_error if socket creation fails.
 */
int		mySocket(int __domain, int __type, int __protocol)
{
	int temp = socket(__domain, __type, __protocol);
	if (temp == -1)
		throw (std::runtime_error("Cant create socket"));
	return (temp);
}

/**
 * @brief Binds a socket to an address and port, with error checking.
 * 
 * Wrapper around the bind() system call that throws a runtime error
 * if binding fails.
 * 
 * @param __fd   The file descriptor of the socket to bind.
 * @param __addr Pointer to the sockaddr structure containing address information.
 * @param __len  Length of the sockaddr structure.
 * 
 * @throws std::runtime_error if binding fails.
 */
void	myBind(int __fd, const sockaddr *__addr, socklen_t __len)
{
	if (bind(__fd, __addr, __len) == -1)
		throw (std::runtime_error("Cant bind to IP/port"));
}

/**
 * @brief Marks a socket as passive to accept incoming connections, with error checking.
 * 
 * Wrapper around the listen() system call that throws a runtime error
 * if setting the socket to listen fails.
 * 
 * @param __fd The file descriptor of the socket to listen on.
 * @param __n  The maximum length for the queue of pending connections.
 * 
 * @throws std::runtime_error if listen fails.
 */
void	myListen(int __fd, int __n)
{
	if (listen(__fd, __n) == -1)
		throw (std::runtime_error("Cant listen"));
}

/**
 * @brief Monitors multiple file descriptors for events, with error checking.
 * 
 * Wrapper around the poll() system call that throws a runtime error
 * if polling fails.
 * 
 * @param __fds    Pointer to an array of pollfd structures.
 * @param __nfds   The number of file descriptors in the array.
 * @param __timeout The timeout in milliseconds (-1 for infinite).
 * 
 * @throws std::runtime_error if poll fails.
 */
void	myPoll(pollfd *__fds, nfds_t __nfds, int __timeout)
{
	if (poll(__fds, __nfds, __timeout) == -1)
		throw (std::runtime_error("Poll failed"));
}

/**
 * @brief Receives data from a socket, with error checking.
 * 
 * Wrapper around the recv() system call that throws a runtime error
 * if receiving data fails.
 * 
 * @param __fd    The file descriptor of the socket to receive data from.
 * @param __buf   Pointer to the buffer to store received data.
 * @param __n     The maximum number of bytes to receive.
 * @param __flags Flags for the recv operation.
 * 
 * @return The number of bytes received.
 * 
 * @throws std::runtime_error if receiving data fails.
 */
size_t	myRecv(int __fd, char *__buf, size_t __n, int __flags)
{
	int temp = recv(__fd, __buf, __n, __flags);
	if (temp == -1)
		throw (std::runtime_error("There was a connection issue"));
	return (temp);
}