/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboulang <lboulang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 19:00:24 by lboulang          #+#    #+#             */
/*   Updated: 2024/05/29 20:26:44 by lboulang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <string.h>
#include "Response.hpp"
#include "Config.hpp"

std::string extractboundary(std::string request_data)
{
	std::string boundary = "";
	std::string::size_type pos = request_data.find("boundary=");
	if (pos != std::string::npos)
	{
		boundary = request_data.substr(pos + 9, request_data.length() - (pos + 9));
		boundary = boundary.substr(0, boundary.find("\r\n"));
	}
	return boundary;
}

void WebServ::receive_request(int client_socket, bool &client_closed_connection, int epoll_fd, std::string &request_data, std::string &boundary)
{
	while (true)
	{
		std::vector<char> buffer(REQUEST_BUFFER_SIZE);
		ssize_t num_bytes_received = recv(client_socket, buffer.data(), buffer.size(), 0);
        if (num_bytes_received <= 0)
        {
			client_closed_connection = true;
			if (num_bytes_received == -1){
				std::cout << "Errno recv() : " << strerror(errno) << std::endl;
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_socket, NULL);
			}
			break;
        }
		request_data.append(buffer.data(), num_bytes_received);//check this for multipart
		if (num_bytes_received < REQUEST_BUFFER_SIZE)
		{
			if (request_data.find("multipart/") != std::string::npos)
			{
				boundary = extractboundary(request_data);
				if (request_data.find(boundary + "--") != std::string::npos)
					break;
				continue;
			}
			break;
		}
		
	}
}

void signalhandler(int signum)
{
	std::cout << "Caught signal " << signum << std::endl;
	WebServ::_sigint = true;
}

void WebServ::SendResponse(Response Resp, int client_socket)
{
	std::string respbuff = Resp.Send();
	ssize_t num_bytes_sent = send(client_socket, respbuff.c_str(), respbuff.size(), 0);
	if (num_bytes_sent == -1)
		throw WebServ::Err("Errno send() : " + std::string(strerror(errno)));
}

std::vector<std::pair<int, std::string> > WebServ::initSocket()
{
	std::vector<std::string> binded_hostport;
	std::vector<std::pair<int, std::string> > _configindexsocket;
	for (size_t i = 0; i < _configlist.size(); ++i)
	{
		Config config = _configlist[i];
		std::string host = config.GetHostFromConfig();

		if (config.GetHostFromConfig() == "localhost")
			host = "127.0.0.1";
			//avoid binding the samehostport 2 times
		if (std::find(binded_hostport.begin(), binded_hostport.end(), config.GetHostPortFromConfig()) != binded_hostport.end()){
			_configindexsocket.push_back(std::make_pair(-1, "-1"));
			continue;
		}

		int server_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (server_socket == -1) {
			throw WebServ::Err("Errno socket() : " + std::string(strerror(errno)));
		}
		int options = 1;
		if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &options, sizeof(options)) == -1){
			close(server_socket);
			throw WebServ::Err("Errno setsockopt() : " + std::string(strerror(errno)));
		}

		//setup host to bind
		struct addrinfo hints, *res;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		if (getaddrinfo(host.c_str(), NULL, &hints, &res) != 0) {
			close(server_socket);
			freeaddrinfo(res);
			throw WebServ::Err("Errno getaddrinfo() : " +host + " : "+ std::string(strerror(errno)));
		}

		//setup host & port as sockaddr_in
		sockaddr_in server_address;
		memset(&server_address, 0, sizeof(server_address));
		server_address.sin_family = AF_INET;
		server_address.sin_port = htons(config.GetPortFromConfig());
		server_address.sin_addr = ((struct sockaddr_in*)res->ai_addr)->sin_addr;

		if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1){
			freeaddrinfo(res);
			close(server_socket);
			throw WebServ::Err("Errno bind() : " + std::string(strerror(errno)));
		}
		freeaddrinfo(res);
		
		if (listen(server_socket, 10) == -1){
			throw WebServ::Err("Errno listen() : " + std::string(strerror(errno)));
		}
		binded_hostport.push_back(config.GetHostPortFromConfig());
		_all_sockets.push_back(server_socket);
		_configindexsocket.push_back(std::make_pair(server_socket, host));
	}

	_epoll_fd = epoll_create(1);
	if (_epoll_fd == -1){
		throw WebServ::Err("Errno epoll_create() : " + std::string(strerror(errno)));
	}

	for (size_t i = 0; i < _all_sockets.size(); ++i) {
		struct epoll_event event;
		event.events = EPOLLIN;
		event.data.fd = _all_sockets[i];
		if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _all_sockets[i], &event) == -1) {
			throw WebServ::Err("Errno epoll_ctl() : " + std::string(strerror(errno)));
		}
	}
	return _configindexsocket;
}

void WebServ::initServer(bool PrintDebug)
{
	std::vector<std::pair<int, std::string> > trustme = initSocket();
	const int MAX_EVENTS = 10; // change  this maybe
	struct epoll_event events[MAX_EVENTS];
	// PrintDebug = false;
	while (WebServ::_sigint == false)
	{
		signal(SIGINT, signalhandler);
		int num_events = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);
		if (num_events == -1)
		{
			if (_sigint == true)
				continue;
			throw WebServ::Err("Errno epoll_wait() : " + std::string(strerror(errno)));
		}
		for (int i = 0; i < num_events; ++i)
		{
			bool is_server_socket = false;
			for (size_t j = 0; j < trustme.size(); ++j) // boucle sur config pour chopper les bons nouveau clients
			{
				if (events[i].data.fd == trustme[j].first)
				{
					_config_nb = j;
					is_server_socket = true;
					break;
				}
			}
			if (is_server_socket)
			{
				sockaddr_in client_address;
				socklen_t client_address_length = sizeof(client_address);
				int client_socket = accept(events[i].data.fd, (struct sockaddr *)&client_address, &client_address_length);
				if (client_socket < 0)
					throw WebServ::Err("Errno accept() : " + std::string(strerror(errno)));
				struct epoll_event event;
				event.events = EPOLLIN | EPOLLET;
				event.data.fd = client_socket;
				if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_socket, &event) == -1)
					throw WebServ::Err("Errno epoll_ctl() : " + std::string(strerror(errno)));
				struct timeval timeout;
				timeout.tv_sec = 1;
				timeout.tv_usec = 0;
				_all_clients_sockets.push_back(client_socket);
				_config_nb_to_host[client_socket] = trustme[_config_nb].first;
				setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
			} 
			else 
			{
				bool stop = false;
				for (size_t x = 0; x < _all_clients_sockets.size(); x++) // boucle sur les clients pour chopper les bons sockets qui ont ete definis dans le if(is_server_socket)
				{
					if (stop)
						break;
					if (_all_clients_sockets[x] == events[i].data.fd)
					{
						_nb_client_socket = x;						
						for (size_t i = 0; i < trustme.size(); i++)
						{
							if (trustme[i].first == _config_nb_to_host[_all_clients_sockets[x]])
							{
								_config_nb = i;
								stop = true;
								break;
							}
						}
					}
				}
				std::string boundary = "";
				std::vector<char> buffer(1024);
				std::string request_data;
				bool client_closed_connection = false;
				
				receive_request(_all_clients_sockets[_nb_client_socket], client_closed_connection, _epoll_fd, request_data, boundary);

				if (client_closed_connection == true){
					close(_all_clients_sockets[_nb_client_socket]);
					_all_clients_sockets.erase(_all_clients_sockets.begin() + _nb_client_socket);
					continue;
				}
				// debug
				if (PrintDebug)
					debug(request_data, "INCOMING REQUEST PARSED", BLUE);
				// not needed if code is cleaner
				if (request_data.empty())
					continue;
		
				Request gabrequest(request_data);
				if (PrintDebug)
					debug(gabrequest, "INCOMING REQUEST PARSED", GREEN);
				
				for (size_t i = 0; i < _configlist.size(); i++)
				{
					if (_configlist[i].GetHostPortFromConfig() == (trustme[_config_nb].second+":"+ToString<int>(_configlist[_config_nb].GetPortFromConfig()))){
						if (gabrequest.getHost() == _configlist[i].GetServerNamesFromConfig()){
							_config_nb = i;
							break;
						}
					}
				}
				Response Resp(gabrequest, _configlist[_config_nb]);
				if (PrintDebug)
					debug(Resp, "RESPONSE CLASS PARSED FROM REQUEST AND CONFIG", CYAN);
				Resp.processRequest();
				if (PrintDebug)
					debug(Resp, "RESPONSE CLASS PROCESSED", CYAN);
				SendResponse(Resp, _all_clients_sockets[_nb_client_socket]);
			}
		}
	}
	for (size_t i = 0; i < _all_sockets.size(); ++i){
		std::cout << "Closing server socket" << _all_sockets[i] << std::endl;
		close(_all_sockets[i]);
	}
	for (size_t i = 0; i < _all_clients_sockets.size(); ++i){
		std::cout << "Closing client socket" << _all_clients_sockets[i] << std::endl;
		close(_all_clients_sockets[i]);
	}
	close(_epoll_fd);
}

bool WebServ::_sigint = false;