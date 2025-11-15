#ifndef DEFINES_HPP
# define DEFINES_HPP

//change the names for ERR_ as IRC, so its simpler
/* 
    all of these need also to send like this
    :<Server Name> <Error code> <Nickname> :<Error Message>
    
*/

#define ERR_PASSWDMISMATCH "464 :Password incorrect"
#define PASSACCEPT "Authenticated, set User and Nick to register"
#define ERR_NOTAUTH "Not authenticated"
#define ERR_NOTREGISTERED "You have not registered"


#endif