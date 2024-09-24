/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcozigon <gcozigon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 17:02:13 by lboulang          #+#    #+#             */
/*   Updated: 2024/05/28 15:49:07 by gcozigon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "Utils.hpp"
#include "Config.hpp"

class Response;

bool checkmethod(std::string method, std::string fullpath) ; //tmp
extern std::map<int, std::string> StatusMap;
extern std::map<std::string, std::string> MIMESTypeMap;
extern std::vector<std::string> AcceptedInstructions;
extern std::vector<std::string> ErrorPages;

void initializeMaps();

class WebServ
{
	public:
		WebServ(){};
		~WebServ(){};
		void loadConfig(char *configFile);
		bool parseConfig(std::ifstream &configFile);
		Config CreateConfigFromServerBlock(std::string &serverBlock, int i);
		
		void initServer(bool PrintDebug);
		Config getConfig() { return _config; };
		void SetNewClientSocket(epoll_event event, int server_socket, int epoll_fd);
		void receive_request(int client_socket, bool &client_closed_connection, int epoll_fd, std::string &request_data, std::string &boundary);
		// Exception class throw custom error message => Server::Err("Error message")
		// void SendResponse(std::string Response, int client_socket);
		void SendResponse(Response Resp, int client_socket);
		std::vector<std::pair<int, std::string> > initSocket();
		class Err : public std::exception
		{
			private:
				std::string message;
			public:
				Err(const std::string &msg) : message(RED + msg + RESET) {}
				~Err() throw() {}
				const char *what() const throw() { return (message.c_str()); }
		};
		static bool	_sigint;
		
	private:
		std::vector<Config> _configlist;
		Config _config;
		int _maxbody;
		int _server_socket;
		int _epoll_fd;
		
	    std::vector<int> _all_sockets;
		std::vector<int> _all_clients_sockets;
		
		int _config_nb;
		int _nb_client_socket;
		std::map<int, int> _config_nb_to_host;

};


template <typename T, typename V>
void debug(T Class, std::string debugmsg, V color)
{
	std::cout << HALFSEP << color << debugmsg << RESET << HALFSEP << std::endl;
	std::cout << Class << std::endl;
	std::cout << SEP << std::endl;
}

std::ostream &operator<<(std::ostream &os, const WebServ &Server);

#endif