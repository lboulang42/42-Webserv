/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboulang <lboulang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 18:29:12 by lboulang          #+#    #+#             */
/*   Updated: 2024/05/28 14:02:13 by lboulang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

/*Return Every Routes For This Server as a Vector*/
std::vector<std::string> Config::GetEveryRoutes(void) const {
	std::vector<std::string> routes;
	for (std::map<std::string, std::map<std::string, std::string> >::const_iterator it = _ServerMap.begin(); it != _ServerMap.end(); it++){
		routes.push_back(it->first);
	}
	return routes;
}

std::vector<std::string> Config::findLongestPrefix(const std::string &path, const std::vector<std::string> &vec) {
	std::vector<std::pair<size_t, std::string> > prefixLengths;
	for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
		size_t i = 0;
		const std::string &str = *it;
		while (i < str.size() && i < path.size() && str[i] == path[i])
			i++;
		if (i == str.size() && (str[i-1] == '/' || i == path.size()))
			prefixLengths.push_back(std::make_pair(i, str));
	}
	
	std::sort(prefixLengths.begin(), prefixLengths.end(), std::greater<std::pair<size_t, std::string> >());
	std::vector<std::string> sortedVec;
	for (size_t i = 0; i < prefixLengths.size(); ++i)
		sortedVec.push_back(prefixLengths[i].second);
	return sortedVec;
}
    
//remove the filename from the uri
std::string Config::GetUriPath(std::string uri) {
	std::size_t pos = uri.find_last_of('/');
	if (pos != std::string::npos) 
		return uri.substr(0, pos + 1);
	return uri;
}

std::string Config::GetKeyFromConfig(const std::string &uri, const std::string &key) {
	std::vector<std::string> vec = GetEveryRoutes();
	vec = findLongestPrefix(uri, vec);
	for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
		if (this->_ServerMap[*it].find(key) != this->_ServerMap[*it].end() && !this->_ServerMap[*it][key].empty()){
			return this->_ServerMap[*it][key];
		}
	}
	return "";
}

Config::Config(Config const&cpy) : 
_ServerMap(cpy._ServerMap),
 _server_names(cpy._server_names),
  _hostport(cpy._hostport),
   _host(cpy._host),
    _portstr(cpy._portstr), 
	_port(cpy._port) 
{
	
}

Config::Config(std::map<std::string, std::map<std::string, std::string> > ServerMap)
{
	for (std::map<std::string, std::map<std::string, std::string> >::iterator it = ServerMap.begin(); it != ServerMap.end(); it++)
	{
		std::cout << "Map For Route : " << it->first << std::endl;
		std::string route = it->first;
		for (std::map<std::string, std::string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
		{
			std::string key = it2->first;
			std::string value = it2->second;
			if (key == "autoindex" && value.empty())
				ServerMap[route]["autoindex"] = "off";
			std::cout << "\t [" << it2->first << "] : " << it2->second << std::endl;
		}
		if (it->first == "/")
		{
			this->_server_names = it->second["server_names"];
			this->_hostport = it->second["listen"];
			this->_host = this->_hostport.substr(0, this->_hostport.find(":"));
			this->_portstr = this->_hostport.substr(this->_hostport.find(":") + 1);
			this->_port = FromString<int>(this->_portstr);
			if (this->_host == "localhost"){
				this->_hostport = "127.0.0.1:"+this->_portstr;
				this->_host = "127.0.0.1";
			}
		}
	}
	this->_ServerMap = ServerMap;
}
