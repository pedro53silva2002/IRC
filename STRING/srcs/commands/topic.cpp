#include "../includes/Server.hpp"

//todo REDOOOOOOOOOOOOOOOOOOOOO
//outputs, numerics, parsing

//broken if client has left the channel (mightve been fixed now with the quit and part changes)
void	Server::commandTopic(int i, std::string line)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())));
		//TODO HAVE A FUNCTION THAT PARSES THIS COMMAND
	if (line.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "TOPIC")));

	
	size_t pos = line.find(' ', 0);
	std::string channelTarget = line.substr(0, pos);
	std::string topic;
	topic = line.substr(pos + 1);
	/* if (pos != std::string::npos)
		topic = line.substr(pos + 1);
	else
		topic = ""; */
	std::cout << "CHANNEL TARGET: " << channelTarget << "\nTOPIC: " << topic << std::endl;
	for (std::vector<Channel>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); ++channelIt)
	{
		if (channelTarget == channelIt->getName())
		{
			if (channelIt->isTopicRestricted() && !_clients[i].getOp())
			{
				std::cout << _clients[i].getNick() << " tried to set topic without being op in channel " << channelTarget << std::endl;
				sendToClient(i, "you cannot set topic in channel " + channelTarget + ": topic is restricted to ops");
				return ;
			}
			channelIt->setTopic(topic);
			std::cout << "Channel " << channelIt->getName() << " topic has been set to \"" << channelIt->getTopic() << "\" by " << _clients[i].getNick() << std::endl;
			sendToClient(i, "you have set the topic in channel " + channelTarget + " to: " + topic);
		}
	}
}