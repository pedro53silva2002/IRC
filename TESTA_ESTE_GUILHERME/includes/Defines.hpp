#ifndef DEFINES_HPP	
# define DEFINES_HPP

#define RPL_WELCOME(nick, network) ("001 " + nick + " :Welcome to "+ network + ", " + nick)
#define RPL_YOURHOST(server) ("002 :Your host is " + server + ", running version 1.0")
#define RPL_MYINFO(server, nick) (server + " 004 " + nick + " 1.0 :Available user modes: io, channel modes: tkl")

#define RPL_CHANNELMODEIS(client, channel) ("324 " + client + " " + channel + " :Available user modes: io, channel modes: tkl")

#define RPL_NOTOPIC(client, channel) ("331 " + client + " " + channel + " :No topic is set")
#define RPL_TOPIC(client, channel, topic) ("332 " + client + " " + channel + " :" + topic)
#define RPL_INVITING(client, nick, channel) ("341 " + client + " " + nick + " :" + channel)
#define RPL_MOTD(nick, motd) ("372 " + nick + ":- " + motd)
#define RPL_MOTDSTART(nick, server) ("375 " + nick + " :- " + server + " Message of the day - ")
#define RPL_ENDOFMOTD(nick) ("376 " + nick + ":End of /MOTD command.")


#define ERR_NOSUCHNICK(client, nick) ("401 " + client + " " + nick + " :No such nick")
#define ERR_NOSUCHCHANNEL(channel) ("403 "  + channel + " :No such channel")
#define ERR_UNKNOWNCOMMAND(client, command) ("421 " + client + " "  + command + " :Unknown command")
#define ERR_NONICKNAMEGIVEN() ("431 :No nickname given")
#define ERR_ERRONEUSNICKNAME(client, nick) ("432 " + client + " " + nick + " :Erroneus nickname")
#define ERR_NICKNAMEINUSE(client, nick) ("433 " + client + " " + nick + " :Nickname is already in use")
#define ERR_USERNOTINCHANNEL(client, nick, channel) ("441 " + client + " " + nick + " " + channel + " :They aren't on that channel")
#define ERR_NOTONCHANNEL(client, channel) ("442 " + client + " " + channel + " :You're not on that channel")
#define ERR_USERONCHANNEL(nick, channel) ("443 " + nick + " " + channel + " :is already on channel\n")
#define ERR_NOTREGISTERED(client) ("451 " + client + " :You are not registered")
#define ERR_NEEDMOREPARAMS(client, command) ("461 " + client + " " + command + " :Not enough parameters")
#define ERR_ALREADYREGISTERED(client) ("462 " + client + " :You may not reregister")
#define ERR_PASSWDMISMATCH(client) ("464 " + client + " :Password incorrect")
#define ERR_CHANNELISFULL(client, channel) ("471 " + client + " " + channel + " :Cannot join channel (+l)")
#define ERR_INVITEONLYCHAN(client, channel) ("473 " + client + " " + channel + " :Cannot join channel (+i)")
#define ERR_BADCHANNELKEY(client, channel) ("475 " + client + " " + channel + " :Cannot join channel (+K)")
#define ERR_BADCHANMASK(client, channel) ("476 " + client + " " + channel + " :Bad Channel Mask")
#define ERR_NOPRIVILEGES(client) ("481 " + client + " :Permission Denied- You're not an IRC operator")
#define ERR_CHANOPRIVSNEEDED(client, channel) ("482 " + client + " " + channel + " :You're not channel operator")
#define ERR_UMODEWUNKNOWNFLAG "501 :Unknown MODE flag"

#define ERR_NOTAUTH "Not authenticated FIX LATER"

#define JOINED(client, channel) (client + " joined " + channel)










#endif
