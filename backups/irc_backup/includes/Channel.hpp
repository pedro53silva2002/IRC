#ifndef CHANNEL_HPP
# define CHANNEL_HPP
# include "Client.hpp"
# include "Colours.hpp"
# include "Server.hpp"

//todo I NEED TO BE SURE TO REMOVE CLIENTS THAT CLOSED SOCKET FROM THIS VECTOR
//!THIS IS SO BAD HONESTLY
//!THIS IS SO BAD HONESTLY
//!THIS IS SO BAD HONESTLY
class Channel
{
	private:
		std::vector<int>	_clientsInChannel;
		std::string			_name;
	public:
		Channel(std::string name) {
			_name = name;
		}
		
		std::vector<int> getClientsInChannel() {
			return (_clientsInChannel);
		}

		void	clientJoin(int id) {
			_clientsInChannel.push_back(id);

			std::cout << "Clients in Channel temp: ";
			for (std::vector<int>::iterator it = _clientsInChannel.begin(); it != _clientsInChannel.end(); it++)
			{
				std::cout << *it << ", ";
			}
			std::cout << std::endl;
		}

		//!THIS IS SO BAD HONESTLY
		//!THIS IS SO BAD HONESTLY
		//!THIS IS SO BAD HONESTLY
		void	sendToClientsInChannel(std::vector<Client> clients, std::string str) ;
		//!THIS IS SO BAD HONESTLY
		//!THIS IS SO BAD HONESTLY
		//!THIS IS SO BAD HONESTLY

};


#endif