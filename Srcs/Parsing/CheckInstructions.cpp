/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckInstructions.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboulang <lboulang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 17:34:47 by lboulang          #+#    #+#             */
/*   Updated: 2024/05/29 20:24:01 by lboulang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parsing.hpp"

/*Check If Str is in IPv4 Format*/
bool isIPv4(const std::string& str) {
	std::stringstream ss(str);
	std::string token;
	std::vector<int> parts;
	while (std::getline(ss, token, '.')) {
		std::stringstream partStream(token);
		int part;
		partStream >> part;
		if (partStream.fail() || part < 0 || part > 255) {
			return false;
		}
		parts.push_back(part);
	}
	return parts.size() == 4;
}

/*Check Listen instruction (host:port)*/
void CheckListen(std::string &hostport){
	std::vector<std::string> hostportvec = splitleo(hostport, ":");
	if (hostportvec.size() != 2)
		throw WebServ::Err("Error: Listen must be formated as host:port\n");
	if (hostportvec[0].empty() || hostportvec[1].empty())
		throw WebServ::Err("Error: Listen must be formated as host:port\n");
	std::string host = hostportvec[0];
	std::string port = hostportvec[1];
	if (host != "localhost" && !isIPv4(host)){
		throw WebServ::Err("Error: Host must be 'localhost' or an IPv4 address\n");
	}
	if (!isNumber(port) || FromString<int>(port) < 1024 || FromString<int>(port) > 65535)
		throw WebServ::Err("Error: Ports Autorized range [1024-65535]\nMore Informations: https://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers ");
}

/*Check root instruction*/
void ConfigCheckRoot(std::string &root){
	if (!isDirectoryReadable(root))
		throw WebServ::Err("Error: Root Directory at "+ root + " is not readable\n");
}

/*Check If Main Server Config Have root, max_body, allowed_methods and listen instructions and if they are correct.*/
void CheckDefaultLocationMap(std::map<std::string,std::string>&map) {
	if (map["root"].empty())
		throw WebServ::Err("Error: No Root Specified in Default Location Block\n");
	if (map["max_body"].empty())
		throw WebServ::Err("Error: No max_body Specified in Default Location Block\n");
	if (map["allowed_methods"].empty())
		throw WebServ::Err("Error: No allowed_methods Specified in Default Location Block\n");
	if (map["listen"].empty())
		throw WebServ::Err("Error: No allowed_methods Specified in Default Location Block\n");
}

void CheckLocationMap(std::map<std::string,std::string>&map){
	if (!map["listen"].empty())
		throw WebServ::Err("Error: Listen Instruction is not allowed in Location Block\n");
	if (!map["server_names"].empty())
		throw WebServ::Err("Error: server_names Instruction is not allowed in Location Block\n");
	if (map["root"].empty())
		throw WebServ::Err("Error: No Root Specified in Location Block\n");
	for (std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); ++it) {//instruction in map
		for (std::vector<std::string>::iterator iter = ErrorPages.begin(); iter != ErrorPages.end(); ++iter) {//if found in ErrorPages; throw
			if (it->first == *iter)
				throw WebServ::Err("Error: " + it->first + " Instruction is not allowed in Location Block\n"	);
		}
	}
}

/*Check max_body instruction and if its positive and < INT_MAX*/
void CheckMaxBody(std::string &max_body){
	if (!isNumber(max_body))
		throw WebServ::Err("Error: 1Max Body must be >0 and < INT_MAX\n");
	long max = FromString<long>(max_body);
	if (max < 0 || max >= (INT_MAX))
		throw WebServ::Err("Error: Max Body must be >0 and < INT_MAX\n");
}

/*Check allowed_methods instruction, parsing and if its GET/POST/DELETE*/
void CheckMethods(std::string &methods) {
	if (methods.length() < 3  || methods.length() > 15 || methods[methods.length() - 1] == '|')
		throw WebServ::Err("allowed_methods : " + methods + "\nAllowed Methods Are GET, POST and DELETE, must be formated as METHOD|METHOD|METHOD\n");
	if (methods.find("|") != std::string::npos) {
		std::vector<std::string> methodsvec = splitleo(methods, "|");
		for (std::vector<std::string>::iterator it = methodsvec.begin(); it != methodsvec.end(); ++it) {
			if (*it != "GET" && *it != "POST" && *it != "DELETE")
				throw WebServ::Err("Allowed Methods Are GET, POST and DELETE, must be formated as METHOD|METHOD|METHOD\n");
		}
	} else {
		if (methods != "GET" && methods != "POST" && methods != "DELETE")
			throw WebServ::Err("Allowed Methods Are GET, POST and DELETE, must be formated as METHOD|METHOD|METHOD\n");
	}
}

/*Check server_names instruction*/
void CheckServerNames(std::string &server_names) {
	for (size_t i = 0; i < server_names.size(); i++) {
		if (!std::isalpha(server_names[i]) && !std::isdigit(server_names[i]) && server_names[i] != '.' && server_names[i] != '-' && server_names[i] != '_')
			throw WebServ::Err("Error: Server Names char must be alpha or .-_ \n");
	}
}

/*Check Location Parameter To Avoid Adding a Route to an invalid uri*/
void CheckLocation(std::string &location) {
	if (location.empty())
		throw WebServ::Err("Error: No Location Specified in Location Block\n");
	for (size_t i = 0; i < location.size(); i++)
		if (!std::isalpha(location[i]) && !std::isdigit(location[i]) && location[i] != '/' && location[i] != '.' && location[i] != '-' && location[i] != '_')
			throw WebServ::Err("Error: Server Names char must be alpha or .-_ \n");
	if (location == "/")
		throw WebServ::Err("Location /, is reserved for the default location\n");
}

/*Check autoindex instruction*/
void CheckAutoIndex(std::string &autoindex) {
	if (autoindex != "on" && autoindex != "off")
		throw WebServ::Err("Error: AutoIndex must be on or off\n");
}

/*Check upload instruction, and if the directory is accessible to write+read*/
void CheckUpload(std::string &uploadpath) {
	if (uploadpath.empty())
		throw WebServ::Err("Error: No Upload Path Specified in Default Location Block\n");
	if (!isDirectoryReadable(uploadpath))
		throw WebServ::Err("Error: Upload Path at " + uploadpath + " is not readable.\n");
	if (!isDirectoryWritable(uploadpath))
		throw WebServ::Err("Error: Upload Path at " + uploadpath + " is not writable.\n");
}

/*Check defaultpage instruction and if the default page is readable*/
void CheckDefaultPage(std::string &root, std::string &defaultpage) {
	if (defaultpage[0] == '/')
		defaultpage.erase(0, 1);
	std::string path = root + defaultpage;
	if (!isFileReadable(path))
		throw WebServ::Err("Error: Default Page at " + path + " is not readable\n");
}

/*Check errors pages instructions (error400, error401 etc.) and Compare with the preloaded vector of alvailables errors status codes*/
void CheckErrorPages(std::map<std::string, std::string> map) {
	for (std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); ++it) {
		std::string value = it->second;
		if (it->first.find("error") == 0){
			for (std::vector<std::string>::iterator iter = ErrorPages.begin(); iter != ErrorPages.end(); ++iter) {
				if (it->first == *iter && !isFileReadable(value))
					throw WebServ::Err("Error Page " + it->first +" at path "+it->second+ " is not readable\n");
			}
		}
	}
}

/*Parse and check CGI instruction (check if cgi is accessible, executable and check the extensions submitted)*/
void ParseConfigCGI(std::vector<std::string> &words, std::map<std::string, std::string>&map){
	if (words.size() != 3)
		throw WebServ::Err("Error: CGI Instruction must be formated as CGI path .ext|.ext|.ext\n");
	if (words[1].empty())
		throw WebServ::Err("Error: CGI " + words[1] + " Is not Executable\n");
	if (access(words[1].c_str(), X_OK))
		throw WebServ::Err("Error: CGI " + words[1] + " Is not Executable\n");
	std::vector<std::string> extensions = splitleo(words[2], "|");
	for (std::vector<std::string>::iterator it = extensions.begin(); it != extensions.end(); ++it) {
		std::string extension = *it;
		if (extension.empty() || extension[0] != '.' || extension.length() < 2) 
			throw WebServ::Err("Error: CGI Extension '" + extension + "' Is not Valid\n");
		extension.erase(0, 1);
		for (size_t i = 0; i < extension.size(); i++) {
			if (!std::isalnum(extension[i]))
				throw WebServ::Err("Error: CGI Extension Name " + extension + " Is not alnum\n");
		}
	}
	map["CGIpath"] = words[1];
	map["CGIext"] = words[2];
}
