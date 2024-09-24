/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboulang <lboulang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 17:35:25 by lboulang          #+#    #+#             */
/*   Updated: 2024/05/28 14:57:11 by lboulang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_HPP
# define PARSING_HPP

#include "Utils.hpp"
#include "Webserv.hpp"


//PARSING CONFIG FILE
void CheckBracketsNumber(std::ifstream &configFile);
void OpenServerBlock(std::string &line, int &bracketCount, int &line_count);
void OutSideServerBlock(std::string &line, int &line_count);
void CloseServerBlock(std::string &line, std::string &currentBlock, int &bracketCount, int &line_count, std::vector<std::string> &serverBlocks);

//EXTRACTING AND PARSING SERVER/LOCATIONS BLOCKS
/*Load The Location Block for the serverblock params into a map*/
std::map<std::string, std::string> ParseLocation(std::string location);
/*Separate ServerBlock into Locationblocks to map routes*/
std::vector<std::string> ExtractLocations(const std::string &serverBlock, int i);



//PARSING INSTRUCTIONS IN CONFIG FILE
/*Check If Str is in IPv4 Format*/
bool isIPv4(const std::string& str);
/*Check Listen instruction (host:port)*/
void CheckListen(std::string &hostport);
/*Check root instruction*/
void ConfigCheckRoot(std::string &root);
/*Check If Main Server Config Have root, max_body, allowed_methods and listen instructions and if they are correct.*/
void CheckDefaultLocationMap(std::map<std::string,std::string>&map);
/*Check Other Locations Config*/
void CheckLocationMap(std::map<std::string,std::string>&map);
/*Check Location Parameter To Avoid Adding a Route to an invalid uri*/
void CheckLocation(std::string &location);
/*Check max_body instruction and if its positive and < LONG_MAX*/
void CheckMaxBody(std::string &max_body);
/*Check allowed_methods instruction, parsing and if its GET/POST/DELETE*/
void CheckMethods(std::string &methods);
/*Check server_names instruction*/
void CheckServerNames(std::string &server_names);
/*Check autoindex instruction*/
void CheckAutoIndex(std::string &autoindex);
/*Check upload instruction, and if the directory is accessible to write+read*/
void CheckUpload(std::string &uploadpath);
/*Check defaultpage instruction and if the default page is readable*/
void CheckDefaultPage(std::string &root, std::string &defaultpage);
/*Check errors pages instructions (error400, error401 etc.) and Compare with the preloaded vector of alvailables errors status codes*/
void CheckErrorPages(std::map<std::string, std::string> map);
/*Parse and check CGI instruction (check if cgi is accessible, executable and check the extensions submitted)*/
void ParseConfigCGI(std::vector<std::string> &words, std::map<std::string, std::string>&map);
#endif