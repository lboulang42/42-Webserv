/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcozigon <gcozigon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 15:00:57 by lboulang          #+#    #+#             */
/*   Updated: 2024/05/29 18:29:02 by gcozigon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <string>
#include "Utils.hpp"

Request::Request(std::string str) : _method(""), _uri(""), _httpversion(""), _body(""), _status_code(0), _payload(""), _isvalid(true), _uriparams(""), _headersmap()
{
	create_map(str);
	_hostport = getHeaderByName("Host");
	_host = _hostport.substr(0, _hostport.find(":"));
	_port = _hostport.substr(_hostport.find(":") + 1);
	if (_host == "localhost"){
		_hostport = "127.0.0.1:"+_port;
		_host = "127.0.0.1";
	}
}

Request::~Request()
{
}

std::vector<std::string> splitleo(const std::string& s, const std::string& delimiter) {
    std::vector<std::string> parts;
    size_t pos = 0;
    size_t len = s.length();
    size_t delim_len = delimiter.length();
    if (delim_len == 0) return parts;
    while (pos < len) {
        int find_pos = s.find(delimiter, pos);
        if (find_pos < 0) {
            parts.push_back(s.substr(pos, len - pos));
            break;
        }
        parts.push_back(s.substr(pos, find_pos - pos));
        pos = find_pos + delim_len;
    }
    return parts;
}

void Request::ParseRequestLine(std::string line) {
	std::vector<std::string> parts = splitleo(line, " ");
	if (parts.size() != 3) {
		_isvalid = false;
		return;
	}
	_method = parts[0];
	_uri = parts[1];
	_httpversion = parts[2];
	if (_method.empty() || _uri.empty() || _httpversion.empty()) {
		_isvalid = false;
		return ;
	}
	if (_method != "GET" && _method != "POST" && _method != "DELETE") {
		_isvalid = false;
		return ; 
	}
	if (_httpversion != "HTTP/1.0" && _httpversion != "HTTP/1.1") {
		_isvalid = false;
		return ;
	}
}

void Request::ParseHeader(std::string headerstr) {
	std::vector<std::string> SplittedHeaders = splitleo(headerstr, "\r\n");
	for (std::vector<std::string>::iterator it = SplittedHeaders.begin(); it != SplittedHeaders.end(); ++it)
	{
		std::cout << "LINE : " << *it << std::endl;
		std::string line = *it;
		if (it == SplittedHeaders.begin()){
			ParseRequestLine(line);
			continue;
		}
		std::vector<std::string> header = splitleo(line, ": ");
		if (header.size() != 2) {
			std::cout << RED << "header is not valid : " << line << RESET << std::endl;
			_isvalid = false;
			return;
		}
		this->_headersmap[header[0]] = header[1];
	}
	
}

void Request::ParseBody(std::string bodystr) {
	_count_crlf--;
	if (_count_crlf >= 1)
	{
		_body.append(bodystr + "\r\n\r\n");
		_payload.append(bodystr + "\r\n\r\n");
	}
	else
	{
		_body.append(bodystr);
		_payload.append(bodystr);
	}
}

void Request::create_map(std::string str)
{
	if (str == "" ) 
        this->_isvalid = false;
    std::string::size_type pos = 0;
    _count_crlf = 0;
    while ((pos = str.find("\r\n\r\n", pos)) != std::string::npos)
	{
        ++_count_crlf;
        pos += 4;
    }
    std::cout << RED << "CRLF : " << _count_crlf << RESET << std::endl;

	std::vector<std::string> splittedheader_body = splitleo(str, "\r\n\r\n");
	for (size_t i = 0; i < splittedheader_body.size(); i++)
	{
		if (i == 0)
			ParseHeader(splittedheader_body[0]);
		else
			ParseBody(splittedheader_body[i]);
	}

	//Reset URI without GET params	
	if (this->_uri.find("?") != std::string::npos)
	{
		this->_uriparams = this->_uri.substr(this->_uri.find("?") + 1);
		this->_uri = this->_uri.substr(0, this->_uri.find("?"));
	}

	//Setup URI Extension
	if (this->_uri.find_last_of(".") != std::string::npos)
		this->_uriextension = this->_uri.substr(this->_uri.find_last_of(".") + 1);
	if (_headersmap["Host"].empty()){
		_isvalid = false;
		std::cout << RED << "Host is empty SEND BACK BAD REQUEST" << RESET << std::endl;
	}
}

std::ostream &operator<<(std::ostream &os, const Request &request)
{
	os << "Method: '" << GREEN << request.getMethod() << RESET << "'" << std::endl;
	os << "URI: '" << GREEN << request.getUri() << RESET << "'" << std::endl;
	os << "HTTP Version: '" << GREEN << request.getHTTPVersion() << RESET << "'" << std::endl;
	os << "Status Code: '" << GREEN << request.getStatusCode() << RESET << "'" << std::endl;
	os << "isvalid :'" << GREEN << request.getIsValid() << RESET << "'" << std::endl;
	os << "uri params :'" << GREEN << request.getUriparams() << RESET << "'" << std::endl;
	os << "URI Extension :'" << GREEN << request.getUriExtension() << RESET << "'" << std::endl;
	os << "Host :'" << GREEN << request.getHost() << RESET << "'" << std::endl;
	os << "Host Port :'" << GREEN << request.getHostPort() << RESET << "'" << std::endl;
	os << "Port :'" << GREEN << request.getPort() << RESET << "'" << std::endl;
	os << "Payload :'" << GREEN << request.getPayload() << RESET << "'" << std::endl; 
	os << "Body :'" << GREEN << request.getBody() << RESET << "'" << std::endl; 
	
	os << request.getHeadersMap() << std::endl;
	return os;
}
