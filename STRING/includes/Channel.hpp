#ifndef CHANNEL_HPP
# define CHANNEL_HPP
# include "Client.hpp"
# include "Colours.hpp"
# include "Server.hpp"

//todo I NEED TO BE SURE TO REMOVE CLIENTS THAT CLOSED SOCKET FROM THIS VECTOR
class Channel
{
	private:
		static int			_globalChannelId;
		int					_id;
		std::string			_name;
		int				    _maxClients;
		int 				_nbrClients;

		std::vector<int>	_clientsInChannel;
	public:
		Channel(std::string name) {
			_id = _globalChannelId++;
			_name = name;
			_maxClients = 0;//default meaning unlimited
			_nbrClients = 0;
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
		
		void			setId(int id) { _id = id; }
		void			setName(std::string name) { _name = name; }

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