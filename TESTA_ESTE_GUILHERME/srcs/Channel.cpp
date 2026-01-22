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
int				Channel::getLimit() {
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
bool				Channel::isOp(int id) {
	if (find(_ops.begin(), _ops.end(), id) != _ops.end())
		return (true);
	return (false);
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
void	Channel::setOp(int id, bool opOrNot) {
	if (opOrNot == true)
		_ops.push_back(id);
	else
		_ops.erase(find(_ops.begin(), _ops.end(), id));
}

//*OTHERS
void	Channel::addClient(int id) {
	_clientsInChannel.push_back(id);
}
void	Channel::removeClient(int id) {
	_clientsInChannel.erase(find(_clientsInChannel.begin(), _clientsInChannel.end(), id));
}