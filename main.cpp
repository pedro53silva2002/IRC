#include "Server.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>       // for getaddrinfo, freeaddrinfo, addrinfo
#include <netinet/in.h>  // for sockaddr_in, htons, INADDR_ANY
#include <arpa/inet.h>   // for inet_ntoa, inet_addr
#include <unistd.h>     // for close
#include <cstring>      // for memset
#include <iostream>
int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << "Error: not enough arguments" << std::endl;
        return 1;
    }
    Server server(argv[1], argv[2]);
    int serverSocket;
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, server.getPort().c_str(), &hints, &res) != 0) {
        std::cerr << "getaddrinfo error" << std::endl;
        exit(1);
    }

    serverSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (serverSocket < 0) {
        std::cerr << "Socket error" << std::endl;
        exit(1);
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(serverSocket, res->ai_addr, res->ai_addrlen) < 0) {
        std::cerr << "Bind error" << std::endl;
        exit(1);
    }

    freeaddrinfo(res);

    if (listen(serverSocket, SOMAXCONN) < 0) {
        std::cerr << "Listen error" << std::endl;
        exit(1);
    }

    std::cout << "Server started on port " << server.getPort() << std::endl;
    std::cout << "Server started with the password " << "<" << server.getPassword() << ">" << std::endl;
    
    /* LOG_SERVER("Server started on port " << port);
    LOG_SERVER("Server started with the password " << "<" << password << ">"); */
    /* while (true) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);

        if (clientSocket < 0) {
            std::cerr << "Accept error" << std::endl;
            continue;
        }

        char buffer[512];
        ssize_t bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            std::cout << "Client sent:\n" << buffer << std::endl;

            // ⚠️ Here we cheat: instead of parsing NICK, assume it's "mynick"
            std::string nick = "peferrei";

            std::string reply =
                ":myserver 001 " + nick + " :Welcome to the IRC server!\r\n"
                ":myserver 002 " + nick + " :Your host is myserver, running version 1.0\r\n"
                ":myserver 003 " + nick + " :This server was created today\r\n"
                ":myserver 004 " + nick + " myserver 1.0 oiwsz biklmnoprstv\r\n";

            send(clientSocket, reply.c_str(), reply.size(), 0);
        }
        close(clientSocket);
    } */
    while (true) {
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);

    if (clientSocket < 0) {
        std::cerr << "Accept error" << std::endl;
        continue;
    }

    char buffer[512];
    ssize_t bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) {
        close(clientSocket);
        continue;
    }
    buffer[bytes] = '\0';
    std::cout << "Client sent: " << buffer << "\t is there anything?" << std::endl;

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

    send(clientSocket, reply.c_str(), reply.size(), 0);

    // Keep the connection open (so HexChat doesn't instantly drop)
    // TODO: implement loop to handle PING/PONG later
    }
    return 0;
}