#ifndef DEFINES_HPP
# define DEFINES_HPP

//change the names for ERR_ as IRC, so its simpler
/* 
    all of these need also to send like this
    :<Server Name> <Error code> <Nickname> :<Error Message>
    
*/

#define ERR_NEEDMOREPARAMS "461 :Not enough parameters"
#define ERR_ALREADYREGISTERED "462 :You may not reregister"
#define ERR_PASSWDMISMATCH "464 :Password incorrect"



#define ERR_NOTAUTH "Not authenticated FIX LATER"
#define ERR_NOTREGISTERED "You have not registered"


#endif