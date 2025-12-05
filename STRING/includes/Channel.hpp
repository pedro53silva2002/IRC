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
		int				    _maxClients;
		int 				_nbrClients;
		bool				_inviteOnly;
		std::string			_channelKey;
		bool				_topicSet;
		std::string			_topic;

		std::vector<int>	_clientsInChannel;
	public:
		Channel(std::string name) {
			_id = _globalChannelId++;
			_name = name;
			_maxClients = 0;//default meaning unlimited
			_nbrClients = 0;
			_inviteOnly = 0;
			_name.erase(std::remove(_name.begin(),_name.end(), '\n'),_name.end());
			_name.erase(std::remove(_name.begin(),_name.end(), '\r'),_name.end());
			_channelKey = "";
			_topicSet = true;
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
		bool			getInviteMode() { return (_inviteOnly); }
		std::string		getChannelKey() { return (_channelKey); }
		bool 			getTopicSet() { return (_topicSet); }
		std::string		getTopic() { return (_topic); }
		
		void			setId(int id) { _id = id; }
		void			setName(std::string name) { _name = name; }
		void			setInviteMode(bool value) { _inviteOnly = value; }
		void			setChannelKey(std::string key) { _channelKey = key; }
		void			setTopicSet(bool value) { _topicSet = value; }
		void			setTopic(std::string topic) { _topic = topic; }
		int			getNbrClients() { return (_nbrClients); }
		int			getMaxClients() { return (_maxClients); }

		void		incrementNbrClients() { 
			//std::cout << "INCREMENTING NBR CLIENTS FROM " << _nbrClients;
			_nbrClients++;}
			//std::cout << " TO " << _nbrClients << std::endl;}
		void		decrementNbrClients() { _nbrClients--; }
		void		setMaxClients(int max) { _maxClients = max; }



};

#endif