#ifndef DEFINES_HPP
# define DEFINES_HPP

/* 
    all of these need also to send like this
    :<Server Name> <Error code> <Nickname> :<Error Message>
    
*/

//check the number
//prob gonna call the numeric in the string itself, and not here, so that <client> and <nick> and <channel> can be sent as well 
#define ERR_USERNOTINCHANNEL "441 :They aren't on that channel"//"<client> <nick> <channel> :They aren't on that channel"
#define ERR_NOTONCHANNEL "442 :You're not on that channel"//"<client> <channel> :You're not on that channel"

#define ERR_NEEDMOREPARAMS "461 :Not enough parameters"
#define ERR_ALREADYREGISTERED "462 :You may not reregister"
#define ERR_PASSWDMISMATCH "464 :Password incorrect"

#define ERR_CHANNELISFULL "471 :Cannot join channel (+l)"//"<client> <channel> :Cannot join channel (+l)"
//unknown mode 472
#define ERR_INVITEONLYCHAN "473 :Cannot join channel (+i)"//"<client> <channel> :Cannot join channel (+i)"
#define ERR_NOPRIVILEGES "481 :Permission Denied- You're not an IRC operator"

#define ERR_UMODEWUNKNOWNFLAG "501 :Unknown MODE flag"

//todo fix
#define ERR_NOTAUTH "Not authenticated FIX LATER"

#endif
