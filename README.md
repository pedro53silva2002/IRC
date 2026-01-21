*This project has been created as part of the 42 curriculum by peferrei.*

# ft_irc - IRC Server

## Description

**ft_irc** is a custom Internet Relay Chat (IRC) server implemented in C++98. The goal of this project is to create a fully functional IRC server that can handle multiple clients simultaneously, allowing users to connect, authenticate, join channels, and communicate with each other in real-time.

The server implements the core IRC protocol, supporting essential features such as:
- **User Authentication**: Password-based server access with PASS, NICK, and USER commands
- **Channel Management**: Create and join channels, set channel topics, and manage channel modes
- **Messaging**: Private messages between users and channel-wide broadcasts
- **Operator Privileges**: Channel operators can manage users and channel settings
- **Channel Modes**: Invite-only, topic restrictions, channel keys (passwords), user limits, and operator status

The server uses `poll()` for handling multiple client connections efficiently, ensuring non-blocking I/O operations.

## Instructions

### Requirements
- A C++ compiler with C++98 support (e.g., `c++`, `g++`, `clang++`)
- Make
- A Unix-like operating system (Linux, macOS)

### Compilation

```bash
cd STRING
make
```

This will compile the server and create the `ircserv` executable.

### Execution

```bash
./ircserv <port> <password>
```

**Parameters:**
- `<port>`: The port number on which the server will listen for incoming connections (e.g., 6667)
- `<password>`: The server password required for client authentication

**Example:**
```bash
./ircserv 6667 mypassword
```

### Connecting to the Server

You can connect using any IRC client (e.g., irssi, WeeChat, HexChat) or netcat:

```bash
nc localhost 6667
```

### Supported Commands

| Command | Description |
|---------|-------------|
| `PASS` | Authenticate with the server password |
| `NICK` | Set or change your nickname |
| `USER` | Set your username and realname |
| `JOIN` | Join a channel |
| `PART` | Leave a channel |
| `PRIVMSG` | Send a private message to a user or channel |
| `TOPIC` | View or set the channel topic |
| `MODE` | Set channel modes (i, t, k, o, l) |
| `KICK` | Remove a user from a channel (operator only) |
| `INVITE` | Invite a user to a channel |
| `QUIT` | Disconnect from the server |

### Cleaning

```bash
make clean   # Remove object files
make fclean  # Remove object files and executable
make re      # Rebuild the project
```

## Resources

### Documentation
- [RFC 1459 - Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459) - The original IRC protocol specification
- [RFC 2812 - Internet Relay Chat: Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812) - Updated IRC client protocol
- [Modern IRC Client Protocol](https://modern.ircdocs.horse/) - Modern documentation for IRC development
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) - Essential guide for socket programming in C/C++

### Tutorials & References
- [IBM - Using poll() instead of select()](https://www.ibm.com/docs/en/i/7.1?topic=designs-using-poll-instead-select) - Understanding poll() for I/O multiplexing
- [GeeksforGeeks - Socket Programming in C/C++](https://www.geeksforgeeks.org/socket-programming-cc/) - Socket programming basics

### AI Usage

AI tools (such as GitHub Copilot and ChatGPT) were used during the development of this project for the following purposes:
- **Documentation**: Assistance in writing and formatting this README file
- **Code Review**: Reviewing code snippets for potential improvements and best practices
- **Debugging**: Help in identifying and understanding error messages and edge cases
- **Research**: Clarifying IRC protocol specifications and socket programming concepts

All AI-generated suggestions were reviewed, tested, and adapted to fit the project requirements and coding standards.