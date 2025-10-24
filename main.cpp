#include "Server.hpp"
#include "Client.hpp"
/* #include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>       // for getaddrinfo, freeaddrinfo, addrinfo
#include <netinet/in.h>  // for sockaddr_in, htons, INADDR_ANY
#include <arpa/inet.h>   // for inet_ntoa, inet_addr
#include <unistd.h>     // for close
#include <cstring>      // for memset
#include <iostream> */
int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << "Error: not enough arguments" << std::endl;
        return 1;
    }
    Server server(argv[1], argv[2]);
    struct addrinfo *res;

    /* //CONFIGURATION OF NETWORK CONNECTION/ SETTING A SOCKET
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;          //IPv4
    hints.ai_socktype = SOCK_STREAM;    //TCP socket
    hints.ai_protocol = IPPROTO_TCP;    //explicitly TCP protocol
    hints.ai_flags = AI_PASSIVE;        //used for listening sockets */

    //Conversion of the port number into a usable network address structure that bind() can understand
    if (getaddrinfo(NULL, server.getPort().c_str(), &server.getHints(), &res) != 0) {
        std::cerr << "getaddrinfo error" << std::endl;
        exit(1);
    }
    server.setRes(res);
    //Creates the actual socket (a file descriptor representing your server connection)
    server.setServerSocket(socket(server.getRes()->ai_family, server.getRes()->ai_socktype, server.getRes()->ai_protocol));
    if (server.getServerSocket() < 0) {
        std::cerr << "Socket error" << std::endl;
        exit(1);
    }

    //lets you restart the server quickly without waiting for the port to be released — common practice for servers
    int opt = 1;
    setsockopt(server.getServerSocket(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    //Attaches the socket to the port number
    if (bind(server.getServerSocket(), server.getRes()->ai_addr, server.getRes()->ai_addrlen) < 0) {
        std::cerr << "Bind error" << std::endl;
        exit(1);
    }

    freeaddrinfo(server.getRes());

    //Marks the socket as a listening socket that will be used to accept incoming connection requests using accept()
    if (listen(server.getServerSocket(), SOMAXCONN) < 0) {
        std::cerr << "Listen error" << std::endl;
        exit(1);
    }

    std::cout << "Server started on port " << server.getPort() << std::endl;
    std::cout << "Server started with the password " << "<" << server.getPassword() << ">" << std::endl;
    
    //Main server loop: accepting clients
    while (true) {
        Client client;
        client.setClientSocket(accept(server.getServerSocket(), (struct sockaddr*)&client.getClientAddr(), &client.getClientLen()));
        //When someone connects, it returns a new socket (clientSocket) that represents that one client
        //int clientSocket = accept(server.getServerSocket(), (struct sockaddr*)&clientAddr, &clientLen);

        if (client.getClientSocket() < 0) {
            std::cerr << "Accept error" << std::endl;
            continue;
        }
        int result = getnameinfo((sockaddr*)&client.getClientAddr(), client.getClientLen(), client.getHost(), NI_MAXHOST, client.getSvc(), NI_MAXSERV, 0);
        if (result)
            std::cout << client.getHost() << " connected on " << client.getSvc() << std::endl;
        else{//this seems stupid
            inet_ntop(AF_INET, &client.getClientAddr(), client.getHost(), NI_MAXHOST);
            std::cout << client.getHost() << " manually connected on " << ntohs() << std::endl;//study ntohs()
        }

        //Reads up to 511 bytes from the client. (IRC lines are usually < 512 bytes)
        char buffer[512];
        ssize_t bytes = recv(client.getClientSocket(), buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            close(client.getClientSocket());
            continue;
        }
        buffer[bytes] = '\0';
        std::cout << "Client sent: " << buffer << "\t got it!" << std::endl;

        // --- Parse nickname from NICK command ---
        std::string input(buffer);
        std::string nick = "peferrei";
        size_t pos = input.find("NICK ");
        if (pos != std::string::npos) {
            size_t start = pos + 5;
            size_t end = input.find("\r\n", start);
            nick = input.substr(start, end - start);
        }

        // --- Send minimal welcome messages ---
        std::string reply =
            ":myserver 001 " + nick + " :Welcome to the IRC server " + nick + "\r\n"
            ":myserver 002 " + nick + " :Your host is myserver, running version 1.0\r\n"
            ":myserver 003 " + nick + " :This server was created today\r\n"
            ":myserver 004 " + nick + " myserver 1.0 oiwsz biklmnoprstv\r\n";

        send(client.getClientSocket(), reply.c_str(), reply.size(), 0);
        // Keep the connection open (so HexChat doesn't instantly drop)
        // TODO: implement loop to handle PING/PONG later
        if (recv(client.getClientSocket(), buffer, sizeof(buffer) - 1, 0) <= 0) {
            close(client.getClientSocket());
            return 0;
        }
        close(client.getClientSocket());
        break;
        //return 0; // --- REMOVE THIS TO KEEP THE SERVER RUNNING ---
    }
    return 0;
}