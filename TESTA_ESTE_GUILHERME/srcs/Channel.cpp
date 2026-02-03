#include "Channel.hpp"

//*CONSTRUCTORS
Channel::Channel() {
	_id = -1;
}
Channel::Channel(std::string name) {
	_id = _globalChannelId++;
	_name = name;
	_name.erase(std::remove(_name.begin(),_name.end(), '\n'),_name.end());
	_name.erase(std::remove(_name.begin(),_name.end(), '\r'),_name.end());
	_limit = 0;
	_channelKey = "";
	_isInviteOnly = false;
	_isTopicRestricted = false;
}
Channel::Channel(const Channel& other) {
	*this = other;
}
Channel& Channel::operator=(const Channel& other) {
	if (this != &other) {
		_id = other._id;
		_name = other._name;
		_channelKey = other._channelKey;
		_limit = other._limit;
		_isInviteOnly = other._isInviteOnly;
		_isTopicRestricted = other._isTopicRestricted;
		_clientsInChannel = other._clientsInChannel;
		_ops = other._ops;
	}
	return (*this);
}
Channel::~Channel() {

}

//*GETTERS
int				Channel::getId() {
	return (_id);
}
std::string		Channel::getName() {
	return (_name);
}
std::string		Channel::getChannelKey() {
	return (_channelKey);
}
size_t				Channel::getLimit() {
	return (_limit);
}
std::string		Channel::getTopic() {
	return (_topic);
}
bool			Channel::isInviteOnly() {
	return (_isInviteOnly);
}
bool 			Channel::isTopicRestricted() {
	return (_isTopicRestricted);
}
std::vector<int>	&Channel::getClientsInChannel() {
	return (_clientsInChannel);
}
std::string		Channel::getTopicTimeSet() {
	return (_topicTimeSet);
}
std::string	Channel::getTopicAuthor() {
	return (_topicAuthor);
}

//*SETTERS
void	Channel::setId(int id) {
	_id = id;
}
void	Channel::setName(std::string name) {
	_name = name;
}
void	Channel::setChannelKey(std::string key) {
	_channelKey = key;
}
void	Channel::setLimit(int limit) {
	_limit = limit;
}
void	Channel::setTopic(std::string topic) {
	_topic = topic;
}
void	Channel::setInviteMode(bool value) {
	_isInviteOnly = value;
}
void	Channel::setTopicRestriction(bool value) {
	_isTopicRestricted = value;
}
void	Channel::setTopicTimeSet() {
	time_t timestamp = time(&timestamp);
	std::stringstream ss;
	ss << timestamp;
	_topicTimeSet = ss.str();
	if (_topicTimeSet.find('\n') != std::string::npos)
		_topicTimeSet.erase(_topicTimeSet.find('\n'));
}
void	Channel::setTopicAuthor(std::string setter) {
	_topicAuthor = setter;
}


//*OTHERS

		bool			isOp(int id);
		bool			isInvited(int id);
		void			setOp(int id, bool opOrNot);
		void			addInvited(int id);

bool	Channel::isOp(int id) {
	if (find(_ops.begin(), _ops.end(), id) != _ops.end())
		return (true);
	return (false);
}
void	Channel::setOp(int id, bool opOrNot) {
	if (opOrNot)
		_ops.push_back(id);
	else
		_ops.erase(find(_ops.begin(), _ops.end(), id));
}
bool	Channel::isInvited(int id) {
	if (find(_invited.begin(), _invited.end(), id) != _invited.end()) {
		std::cout << "is invited??\n";
		return (true);
	}
	return (false);
}
void	Channel::addInvited(int id) {
	_invited.push_back(id);
}



void		Channel::decrementId() {
	_globalChannelId--;
}
void	Channel::addClient(int id) {
	_clientsInChannel.push_back(id);
}
void	Channel::removeClient(int id) {
	_clientsInChannel.erase(find(_clientsInChannel.begin(), _clientsInChannel.end(), id));
	std::vector<int>::iterator pos = find(_invited.begin(), _invited.end(), id);
	if (pos != _invited.end())
		_invited.erase(pos);
	pos = find(_ops.begin(), _ops.end(), id);
	if (pos != _ops.end())
		_ops.erase(pos);
}