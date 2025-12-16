#ifndef DEFINES_HPP	
# define DEFINES_HPP

//SERVER PREFIX MISSING
//also, check all outputs and numerics
#define ERR_NOSUCHCHANNEL(client, channel) ("403 " + client + " "  + channel + " :No such channel")
#define ERR_UNKNOWNCOMMAND(client, command) ("421 " + client + " "  + command + " :Unknown command")
#define ERR_NONICKNAMEGIVEN(client) ("431 " + client + " :No nickname given")
#define ERR_ERRONEUSNICKNAME(client, nick) ("432 " + client + " " + nick + " :Erroneus nickname")
#define ERR_NICKNAMEINUSE(client, nick) ("433 " + client + " " + nick + " :Nickname is already in use")
#define ERR_USERNOTINCHANNEL(client, nick, channel) ("441 " + client + " " + nick + " " + channel + " :They aren't on that channel")
#define ERR_NOTONCHANNEL(client, channel) ("442 " + client + " " + channel + " :You're not on that channel")
#define ERR_NOTREGISTERED(client) ("451 " + client + " :You are not registered")
#define ERR_NEEDMOREPARAMS(client, command) ("461 " + client + " " + command + " :Not enough parameters")
#define ERR_ALREADYREGISTERED(client) ("462 " + client + " :You may not reregister")
#define ERR_PASSWDMISMATCH(client) ("464 " + client + " :Password incorrect")
#define ERR_CHANNELISFULL(client, channel) ("471 " + client + " " + channel + " :Cannot join channel (+l)")
//unknown mode 472
#define ERR_INVITEONLYCHAN(client, channel) ("473 " + client + " " + channel + " :Cannot join channel (+i)")
#define ERR_BADCHANNELKEY(client, channel) ("475 " + client + " " + channel + " :Cannot join channel (+K)")
#define ERR_BADCHANMASK(client, channel) ("476 " + client + " " + channel + " :Bad Channel Mask")
#define ERR_NOPRIVILEGES(client) ("481 " + client + " :Permission Denied- You're not an IRC operator")
#define ERR_UMODEWUNKNOWNFLAG "501 :Unknown MODE flag"//TODO

//todo fix
#define ERR_NOTAUTH "Not authenticated FIX LATER"

//join
#define JOINED(client, channel) (client + " joined " + channel)
#define RPL_TOPIC(client, channel, topic) ("332 " + client + " " + channel + " :" + topic)

//MODE +o
#define OPERATOR(client, channel, nick) (client + " MODE " + channel + " +o " + nick)









#endif
