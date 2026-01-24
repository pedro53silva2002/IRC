*This project has been created as part of the 42 curriculum by brfernan and peferrei.*

# IRC Server Project

## Description

This project is an implementation of a simplified IRC (Internet Relay Chat) server, developed as part of the 42 curriculum. The goal is to understand and apply network programming concepts, client-server architecture, and the IRC protocol. The server manages multiple clients, channels, and supports core IRC commands such as JOIN, PART, PRIVMSG, KICK, INVITE, and more. The project is structured in C++ and emphasizes modularity, code clarity, and adherence to the IRC RFC standards.

## Instructions

### Compilation

To compile the project, ensure you have a C++ compiler installed (e.g., `g++`). Then, run:

```zsh
make
```

This will build the server executable according to the rules defined in the `Makefile`.

### Execution

To run the IRC server, use:

```zsh
./ircserv <port> <password>
```

- `<port>`: The port number the server will listen on.
- `<password>`: The password required for clients to connect.

Example:

```zsh
./ircserv 6667 mypassword
```

## Resources

- [RFC 1459: Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
- [RFC 2812: Internet Relay Chat: Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)


### Use of AI

AI assistance (GitHub Copilot, GPT-4.1) was used for:
- Generating boilerplate code and documentation.
- Drafting this README and ensuring clarity in instructions and descriptions.

For any further information, please refer to the source code and comments within the project files.
