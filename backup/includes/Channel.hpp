#ifndef CHANNEL_HPP
# define CHANNEL_HPP
# include "Client.hpp"
# include "Colours.hpp"
# include "Server.hpp"
# include <iostream>
# include <sys/types.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <string.h>
# include <string>
# include <vector>
# include <poll.h>
# include <stdlib.h>//atoi
# include <algorithm>

//todo I NEED TO BE SURE TO REMOVE CLIENTS THAT CLOSED SOCKET FROM THIS VECTOR
class Channel
{
	private:
		static int			_globalChannelId;
		int					_id;
		std::string			_name;
		
		int 				_nbrClients;
		std::string			_channelKey;
		int				    _limit;
		std::string			_topic;
		bool				_isInviteOnly;
		bool				_isTopicRestricted;

		std::vector<int>	_clientsInChannel;
	public:
		Channel(std::string name) {
			_id = ++_globalChannelId;
			_name = name;
			_name.erase(std::remove(_name.begin(),_name.end(), '\n'),_name.end());//wqhat??
			_name.erase(std::remove(_name.begin(),_name.end(), '\r'),_name.end());
			_limit = 0;//default meaning unlimited
			_nbrClients = 0;
			_channelKey = "";
			_isInviteOnly = false;
			_isTopicRestricted = true;//check
		}

		Channel() {
			_id = -1;
		}//DUMMY
		
		std::vector<int> getClientsInChannel() {
			return (_clientsInChannel);
		}

		//todo hardcoded
		void	clientJoin(int id) {
			_clientsInChannel.push_back(id);
		}
		int				getId() { return (_id); }
		std::string		getName() { return (_name); }
		std::string		getChannelKey() { return (_channelKey); }
		std::string		getTopic() { return (_topic); }
		bool			isInviteOnly() { return (_isInviteOnly); }
		bool 			isTopicRestricted() { return (_isTopicRestricted); }
		
		void			setId(int id) { _id = id; }
		void			setName(std::string name) { _name = name; }
		void			setChannelKey(std::string key) { _channelKey = key; }
		void			setTopic(std::string topic) { _topic = topic; }
		void			setInviteMode(bool value) { _isInviteOnly = value; }
		void			setTopicRestriction(bool value) { _isTopicRestricted = value; }
		int				getNbrClients() { return (_nbrClients); }
		int				getLimit() { return (_limit); }

		void		incrementNbrClients() { _nbrClients++; }
		void		decrementNbrClients() { _nbrClients--; }
		void		setLimit(int limit) { _limit = limit; }



};

#endif