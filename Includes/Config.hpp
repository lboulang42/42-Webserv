/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboulang <lboulang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 00:32:22 by lboulang          #+#    #+#             */
/*   Updated: 2024/05/28 13:59:45 by lboulang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP
#include "Utils.hpp"
#include <map>

class Config
{
	public:
		Config(std::map<std::string, std::map<std::string, std::string> > ServerMap);
		~Config(){};
		Config(Config const&);
		Config(){};//temp
		
		///////////////////////////////////OLD NEED TO TEJ//////////////////////////////////
		
		int getMaxbody() { return _maxbody; };//tmp;
		int _maxbody; //tmp
		
		/*//////////////GETTERS//////////////////////*/
		int			GetPortFromConfig() const { return _port; };
		std::string GetServerNamesFromConfig() { return _server_names; };
		std::string GetHostPortFromConfig() const { return _hostport; };
		std::string GetHostFromConfig() const { return _host; };
		std::string GetPortStrFromConfig() const { return _portstr; };
		
		std::string GetRedirect(const std::string &location) { 
			if (_ServerMap[location].empty() || _ServerMap[location]["redirect"].empty())
				return "";
			return _ServerMap[location]["redirect"];
		};
		std::vector<std::string> GetAllowedMethodsFromConfig(const std::string &uri) { 
			std::string methodsasstr = GetKeyFromConfig(uri, "allowed_methods");
			std::vector<std::string>methods;
			if (methodsasstr.find('|') != std::string::npos)
				methods = splitleo(methodsasstr, "|");
			else 
				methods.push_back(methodsasstr);
			return methods; 
		}
		std::string GetCGIPathFromConfig(const std::string &uri) { return GetKeyFromConfig(uri, "CGIpath"); };
		std::string GetCGIExtFromConfig(const std::string &uri) { return GetKeyFromConfig(uri, "CGIext"); };
		std::string GetRootDirFromConfig(const std::string &uri) { return GetKeyFromConfig(uri, "root"); }
		std::string GetUploadFromConfig(const std::string &uri) { return GetKeyFromConfig(uri, "upload"); }
		std::string GetMaxBodyFromConfig(const std::string &uri) { return GetKeyFromConfig(uri, "max_body"); }
		std::string GetAutoIndexFromConfig(const std::string &uri) { 
			return GetKeyFromConfig(uri, "autoindex");
		};
		std::string GetDefaultPageFromConfig(const std::string &uri) {
			return GetKeyFromConfig(uri, "defaultpage");
		};
		std::string GetLocationFromConfig(const std::string &fullpath) { 
			std::vector<std::string> vec = GetEveryRoutes();
			vec = findLongestPrefix(fullpath, vec);
			if (!vec.empty())
				return vec[0];
			return "";
		}
		std::string GetDefaultPageNew(const std::string &fullpath) {
			std::vector<std::string> vec = GetEveryRoutes();
			for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++) {
				if (*it == fullpath)
					if (_ServerMap[*it]["defaultpage"] != "")
						return _ServerMap[*it]["location"] + _ServerMap[*it]["defaultpage"];
			}
			return "";
		}
		std::string GetErrorPage(const std::string &uri, const int &status) { return GetKeyFromConfig(uri, "error"+ToString<int>(status)); }
		std::string GetDefaultPage(const std::string &uri) { 
			return GetKeyFromConfig(uri, "defaultpage");
		};
		bool CanUpload(const std::string &uri) { 
			if (GetUploadFromConfig(uri).empty())
				return false;	
			return true;
		};
		
		std::vector<std::string> GetSortedLocationList(const std::string &uri){
			std::vector<std::string> vec = GetEveryRoutes();
			vec = findLongestPrefix(uri, vec);
			return vec;
		}
		/*//////////////UTILS//////////////////////*/
		//remove the filename from the uri
		std::string GetUriPath(std::string uri);
		std::string GetKeyFromConfig(const std::string &uri, const std::string &key);
		/*Return Every Routes For This Server as a Vector*/
		std::vector<std::string> GetEveryRoutes(void) const;
		/*Return the list of Routes sorted for a path / uri */
		std::vector<std::string> findLongestPrefix(const std::string &path, const std::vector<std::string> &vec);

private:
	std::map<std::string, std::map<std::string, std::string> > _ServerMap;
	/*Variable constant for the whole server block*/
	std::string _server_names;
	std::string _hostport;
	std::string _host;
	std::string _portstr;
	int _port;
};

#endif