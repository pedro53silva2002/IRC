#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Utils.hpp"

class Channel
{
	private:
		static int			_globalChannelId;
		int					_id;
		std::string			_name;
		std::string			_channelKey;
		int				    _limit;
		std::string			_topic;
		bool				_isInviteOnly;
		bool				_isTopicRestricted;
		std::vector<int>	_clientsInChannel;
		std::vector<int>	_ops;
	public:
		Channel(std::string name);
		Channel();
		Channel(const Channel& other);
		Channel& operator=(const Channel& other);
		

		int				getId();
		std::string		getName();
		std::string		getChannelKey();
		std::string		getTopic();
		bool			isInviteOnly();
		bool 			isTopicRestricted();
		int				getLimit();
		std::vector<int> &getClientsInChannel();
		bool		isOp(int id);
		
		void			setId(int id);
		void			setName(std::string name);
		void			setChannelKey(std::string key);
		void			setTopic(std::string topic);
		void			setInviteMode(bool value);
		void			setTopicRestriction(bool value);
		void		setLimit(int limit);
		void		setOp(int id, bool opOrNot);




		void	addClient(int id);
		void	removeClient(int id);
};

#endif