#ifndef DEFINES_HPP	
# define DEFINES_HPP

// :<Server Name> <Numeric> <Nickname> :<Error Message>
//SERVER PREFIX MISSING
#define ERR_NOSUCHCHANNEL(client, channel) ("403 " + client + " "  + channel + " :No such channel")//*done
#define ERR_UNKNOWNCOMMAND(client, command) ("421 " + client + " "  + command + " :Unknown command")//*done
#define ERR_NONICKNAMEGIVEN(client) ("431 " + client + " :No nickname given")//*done
#define ERR_ERRONEUSNICKNAME(client, nick) ("432 " + client + " " + nick + " :Erroneus nickname")//*done
#define ERR_USERNOTINCHANNEL(client, nick, channel) ("441 " + client + " " + nick + " " + channel + " :They aren't on that channel")//*done
#define ERR_NOTONCHANNEL(client, channel) ("442 " + client + " " + channel + " :You're not on that channel")//*done
#define ERR_NEEDMOREPARAMS(client, command) ("461 " + client + " " + command + " :Not enough parameters")//*done
#define ERR_ALREADYREGISTERED(client) ("462 " + client + " :You may not reregister")//*done
#define ERR_PASSWDMISMATCH(client) ("464 " + client + " :Password incorrect")//*done
#define ERR_CHANNELISFULL(client, channel) ("471 " + client + " " + channel + " :Cannot join channel (+l)")//*done
//unknown mode 472
#define ERR_INVITEONLYCHAN(client, channel) ("473 " + client + " " + channel + " :Cannot join channel (+i)")//*done
#define ERR_BADCHANNELKEY(client, channel) ("475 " + client + " " + channel + " :Cannot join channel (+K)")//*done
#define ERR_NOPRIVILEGES(client) ("481 " + client + " :Permission Denied- You're not an IRC operator")//*done
#define ERR_UMODEWUNKNOWNFLAG "501 :Unknown MODE flag"//TODO

//todo fix
#define ERR_NOTAUTH "Not authenticated FIX LATER"

//join
#define JOINED(client, channel) (client + " joined " + channel)//*done
#define RPL_TOPIC(client, channel, topic) ("332 " + client + " " + channel + " :" + topic)//*done

//MODE +o
#define OPERATOR(client, channel, nick) (client + " MODE " + channel + " +o " + nick)//*done









#endif
