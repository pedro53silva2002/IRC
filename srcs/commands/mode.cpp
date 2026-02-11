#include "../includes/Server.hpp"

bool	Server::isValidMode(int i, std::string args)
{
	if (!_clients[i].isRegistered())
		return (sendToClient(i, ERR_NOTREGISTERED(_clients[i].getNick())), false);
	if (args.empty())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "MODE")), false);
	return (true);
}

void	Server::modeInviteOnly(int i, int chId, bool *enableMode)
{
	_channels[chId].setInviteMode(*enableMode);
	sendToClient(i, MODE1(_clients[i].getNick(), _channels[chId].getName(), (*enableMode ? '+' : '-') + 'i'));
	if (!*enableMode)
		*enableMode = true;
}

void	Server::modeTopicRestriction(int i, int chId, bool *enableMode)
{
	_channels[chId].setTopicRestriction(*enableMode);
	sendToClient(i, MODE1(_clients[i].getNick(), _channels[chId].getName(), (*enableMode ? '+' : '-') + 't'));
	if (!*enableMode)
		*enableMode = true;
}

void	Server::modeKey(int i, int chId, std::vector<std::string> args, bool *enableMode, size_t *argsIdx)
{
	if (!*enableMode) {
		_channels[chId].setChannelKey("");
		sendToClient(i, MODE1(_clients[i].getNick(), _channels[chId].getName(), "-k"));
		*enableMode = true;
		return ;
	}
	if (*argsIdx >= args.size())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "MODE")));
	_channels[chId].setChannelKey(args[*argsIdx]);
	sendToClient(i, MODE2(_clients[i].getNick(), _channels[chId].getName(), "+k", args[*argsIdx]));
	(*argsIdx)++;
}

void	Server::modeOp(int i, int chId, std::vector<std::string> args, bool *enableMode, size_t *argsIdx)
{
	if (*argsIdx >= args.size())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "MODE")));
	std::transform(args[*argsIdx].begin(), args[*argsIdx].end(), args[*argsIdx].begin(), ::tolower);
	int toOpId = getClientId(args[*argsIdx]);
	if (toOpId == -1)
		return (sendToClient(i, ERR_NOSUCHNICK(_clients[i].getNick(), args[*argsIdx])));
	if (!isUserInChannel(toOpId, chId))
		return (sendToClient(i, ERR_USERNOTINCHANNEL(_clients[i].getNick(), args[*argsIdx], _channels[chId].getName())));
	_channels[chId].setOp(toOpId, *enableMode);
	channelBroadcast(chId, MODE2(_clients[i].getNick(), _channels[chId].getName(), (*enableMode ? '+' : '-') + 'o', args[*argsIdx]));
	if (!*enableMode)
		*enableMode = true;
	(*argsIdx)++;
}

void	Server::modeLim(int i, int chId, std::vector<std::string> args, bool *enableMode, size_t *argsIdx)
{
	if (!*enableMode) {
		_channels[chId].setLimit(0);
		return (sendToClient(i, MODE1(_clients[i].getNick(), _channels[chId].getName(), "-l")));
	}
	else if (*argsIdx >= args.size())
		return (sendToClient(i, ERR_NEEDMOREPARAMS(_clients[i].getNick(), "MODE")));
	else
		_channels[chId].setLimit(atoi(args[*argsIdx].c_str()));
	
	sendToClient(i, MODE2(_clients[i].getNick(), _channels[chId].getName(), "+l", args[*argsIdx]));
	if (!*enableMode)
		*enableMode = true;
	(*argsIdx)++;
}

void	Server::commandMode(int i, std::string line)
{
	if (!isValidMode(i, line))
		return ;
	
	std::vector<std::string> args = getArgs(line);
	std::transform(args[0].begin(), args[0].end(), args[0].begin(), ::tolower);
	int chId = getChannelId(args[0]);
	if (!isUserInChannel(i, chId))
		return (sendToClient(i, ERR_NOTONCHANNEL(_clients[i].getNick(), args[0])));
	if (args.size() < 2)
		return (sendToClient(i, RPL_CHANNELMODEIS(_clients[i].getNick(), args[0])));
	if (!_channels[chId].isOp(i))
		return (sendToClient(i, ERR_CHANOPRIVSNEEDED(_clients[i].getNick(), args[0])));

	size_t j = 0;
	size_t	argIdx = 2;
	bool	enableMode = (args[1][0] != '-');
	if (args[1][0] == '+' || args[1][0] == '-')
		j++;
	for (; j < args[1].length(); j++) {
		switch (args[1][j]) {
			case 'i':
				modeInviteOnly(i, chId, &enableMode);
				break ;
			case 't':
				modeTopicRestriction(i, chId, &enableMode);
				break ;
			case 'k':
				modeKey(i, chId, args, &enableMode, &argIdx);
				break ;
			case 'o':
				modeOp(i, chId, args, &enableMode, &argIdx);
				break ;
			case 'l':
				modeLim(i, chId, args, &enableMode, &argIdx);
				break ;
			case '+':
				enableMode = true;
				break ;
			case '-':
				enableMode = false;
				break ;
			default:
				sendToClient(i, ERR_UMODEWUNKNOWNFLAG(args[1][j]));
				break;
		}
	}
}