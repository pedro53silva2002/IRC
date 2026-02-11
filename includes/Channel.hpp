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
		std::string 		_topicTimeSet;
		std::string			_topicAuthor;
		bool				_isInviteOnly;
		bool				_isTopicRestricted;
		std::vector<int>	_clientsInChannel;
		std::vector<int>	_ops;
		std::vector<int>	_invited;
	
	public:
		//*CONSTRUCTORS
		Channel(std::string name);
		Channel();
		Channel(const Channel& other);
		Channel& operator=(const Channel& other);
		~Channel();
		
		//*GETTERS
		int				getId();
		std::string		getName();
		std::string		getChannelKey();
		std::string		getTopic();
		std::string		getTopicTimeSet();
		std::string		getTopicAuthor();
		bool			isInviteOnly();
		bool 			isTopicRestricted();
		size_t			getLimit();
		std::vector<int> &getClientsInChannel();
		
		//*SETTERS
		void	setId(int);
		void	setName(std::string);
		void	setChannelKey(std::string);
		void	setTopic(std::string);
		void	setTopicTimeSet();
		void	setTopicAuthor(std::string);
		void	setInviteMode(bool);
		void	setTopicRestriction(bool);
		void	setLimit(int);
		
		//*OTHERS
		bool	isOp(int);
		void	setOp(int, bool);
		bool	isInvited(int);
		void	addInvited(int);
		void	decrementId();
		void	addClient(int);
		void	removeClient(int);
};

#endif