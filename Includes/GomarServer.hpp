/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maheraul <maheraul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 22:56:05 by maheraul          #+#    #+#             */
/*   Updated: 2024/05/15 20:46:07 by maheraul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GOMARSERVER_CPP
#define GOMARSERVER_CPP

#include <iostream>
#include <stdexcept>
#include <map>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>
#include <sys/stat.h>
#include <unistd.h>
#include "Webserv.hpp"

std::string readServerBlock(const std::string &filename, int pos);
int countServerBlocks(const std::string &filename);

class GomarServer
{
private:
    std::string _serverName;
    std::string _listen;
    std::string _host;
    std::string _port;
    std::string _root;
    std::string _index;
    std::string _allowed_methods;
    std::string _filepath_post;
    std::string _max_body_size;
    std::string _redirect;
    std::string _key;
    std::string _block;
    std::string _block_serv;
    std::string _block_location;
    std::map<std::string, GomarServer> _locations;
    void extractLocations();
    std::string extract_Value(const std::string &memberName, const std::string &block);
    std::string readLocationBlock(const std::string &text);
    std::string cutLocation(const std::string &text);
    void is_alpha(const std::string &str, const std::string &member) const;
    void listen_ok(const std::string &ip, const std::string &port, const std::string &member) const;
    void parseConfig();
    void setHostPort();
    void root_ok(const std::string &root);
    void index_ok(const std::string &index, const std::string &member);
    void filepath_post_ok(const std::string &filepath_post, const std::string &member);

public:
    GomarServer();
    GomarServer(const std::string &block, const std::string &block_location, const std::string &key);
    ~GomarServer();
    void printLocationBlocks() const;
    void setBlock(const std::string &text);
    void printValue(void);
    void addLocation(const GomarServer &block);
    void setUp(const std::string &text);
    std::string get(const std::string &key, const std::string &memberName);
    std::string get_default(const std::string &memberName);
};

void print_conf(GomarServer *conf_array, int array_size);

#endif