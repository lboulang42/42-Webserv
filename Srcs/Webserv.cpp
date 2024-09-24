/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboulang <lboulang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 16:10:38 by lboulang          #+#    #+#             */
/*   Updated: 2024/05/28 13:23:33 by lboulang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

// https://datatracker.ietf.org/doc/html/rfc2616#section-10.1.1
std::map<int, std::string> StatusMap;
std::map<std::string, std::string> MIMESTypeMap;
std::vector <std::string> AcceptedInstructions;
std::vector <std::string> ErrorPages;

void WebServ::loadConfig(char *configFile) {
	if (access(configFile, R_OK) != EXIT_SUCCESS)
		throw WebServ::Err("'" + std::string(configFile) + "' as config file : " + std::string(strerror(errno)));
	std::ifstream configStream(configFile);
	if (!configStream.is_open()) // double check to load ifstream, this condition should never be true
		throw WebServ::Err("Error opening '" + std::string(configFile) + "' as config file");
	// maybe check file extension
	if (!parseConfig(configStream))
		throw WebServ::Err("'" + std::string(configFile) + "' as config file : Unable to load config file");
}

void initializeMaps()
{
	std::cout << "Initializing Status Map Called" << std::endl;
	StatusMap[100] = "Continue";
	StatusMap[101] = "Switching Protocols";

	StatusMap[200] = "OK";
	StatusMap[201] = "Created";
	StatusMap[202] = "Accepted";
	StatusMap[203] = "Non-Authoritative Information";
	StatusMap[204] = "No Content";
	StatusMap[205] = "Reset Content";
	StatusMap[206] = "Partial Content";

	StatusMap[300] = "Multiple Choices";
	StatusMap[301] = "Mooved Permanently";
	StatusMap[302] = "Found";
	StatusMap[303] = "See Other";
	StatusMap[304] = "Not Modified";
	StatusMap[305] = "Use Proxy";
	StatusMap[306] = "(Unused)";
	StatusMap[307] = "Temporary Redirect";

	StatusMap[400] = "Bad Request";
	StatusMap[401] = "Unauthorized";
	StatusMap[402] = "Payment Required";
	StatusMap[403] = "Forbidden";
	StatusMap[404] = "Not Found";
	StatusMap[405] = "Method Not Allowed";
	StatusMap[406] = "Not Acceptable";
	StatusMap[407] = "Proxy Authentication Required";
	StatusMap[408] = "Request Timeout";
	StatusMap[409] = "Conflict";
	StatusMap[410] = "Gone";
	StatusMap[411] = "Length Required";
	StatusMap[412] = "Precondition Failed";
	StatusMap[413] = "Request Entity Too Large";
	StatusMap[414] = "Request-URI Too Long";
	StatusMap[415] = "Unsupported Media Type";
	StatusMap[416] = "Requested Range Not Satisfiable";
	StatusMap[417] = "Expectation Failed";

	StatusMap[500] = "Internal Server Error";
	StatusMap[501] = "Not Implemented";
	StatusMap[502] = "Bad Gateway";
	StatusMap[503] = "Service Unavailable";
	StatusMap[504] = "Gateway Timeout";
	StatusMap[505] = "HTTP Version Not Supported";

	MIMESTypeMap["html"] = "text/html";
	MIMESTypeMap["css"] = "text/css";
	MIMESTypeMap["js"] = "application/javascript";
	MIMESTypeMap["json"] = "application/json";
	MIMESTypeMap["jpeg"] = "image/jpeg";
	MIMESTypeMap["jpg"] = "image/jpg";
	MIMESTypeMap["png"] = "image/png";
	MIMESTypeMap["gif"] = "image/gif";
	MIMESTypeMap["svg"] = "image/svg+xml";
	MIMESTypeMap["webp"] = "image/webp";
	MIMESTypeMap["ico"] = "image/x-icon";
	MIMESTypeMap["pdf"] = "application/pdf";
	MIMESTypeMap["webm"] = "video/webm";
	MIMESTypeMap["mp4"] = "video/mp4";
	MIMESTypeMap["mp3"] = "audio/mpeg";
	MIMESTypeMap["wav"] = "audio/wav";
	MIMESTypeMap["txt"] = "text/plain";
	MIMESTypeMap["xml"] = "application/xml";
	MIMESTypeMap["zip"] = "application/zip";
	MIMESTypeMap["gzip"] = "application/gzip";
	
	AcceptedInstructions.push_back("listen");
	AcceptedInstructions.push_back("max_body");
	AcceptedInstructions.push_back("allowed_methods");
	AcceptedInstructions.push_back("root");
	AcceptedInstructions.push_back("server_names");
	AcceptedInstructions.push_back("error400");
	AcceptedInstructions.push_back("error401");
	AcceptedInstructions.push_back("error402");
	AcceptedInstructions.push_back("error403");
	AcceptedInstructions.push_back("error404");
	AcceptedInstructions.push_back("error405");
	AcceptedInstructions.push_back("error406");
	AcceptedInstructions.push_back("error407");
	AcceptedInstructions.push_back("error408");
	AcceptedInstructions.push_back("error409");
	AcceptedInstructions.push_back("error410");
	AcceptedInstructions.push_back("error411");
	AcceptedInstructions.push_back("error412");
	AcceptedInstructions.push_back("error413");
	AcceptedInstructions.push_back("error414");
	AcceptedInstructions.push_back("error415");
	AcceptedInstructions.push_back("error416");
	AcceptedInstructions.push_back("error417");
	AcceptedInstructions.push_back("error500");
	AcceptedInstructions.push_back("error502");
	AcceptedInstructions.push_back("error504");
	AcceptedInstructions.push_back("redirect");
	AcceptedInstructions.push_back("autoindex");
	AcceptedInstructions.push_back("defaultpage");
	AcceptedInstructions.push_back("upload");
	AcceptedInstructions.push_back("CGI");

	ErrorPages.push_back("error400");
	ErrorPages.push_back("error401");
	ErrorPages.push_back("error402");
	ErrorPages.push_back("error403");
	ErrorPages.push_back("error404");
	ErrorPages.push_back("error405");
	ErrorPages.push_back("error406");
	ErrorPages.push_back("error407");
	ErrorPages.push_back("error408");
	ErrorPages.push_back("error409");
	ErrorPages.push_back("error410");
	ErrorPages.push_back("error411");
	ErrorPages.push_back("error412");
	ErrorPages.push_back("error413");
	ErrorPages.push_back("error414");
	ErrorPages.push_back("error415");
	ErrorPages.push_back("error416");
	ErrorPages.push_back("error417");
}