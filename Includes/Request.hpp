/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcozigon <gcozigon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 15:40:36 by lboulang          #+#    #+#             */
/*   Updated: 2024/05/29 18:14:21 by gcozigon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP
#include "Webserv.hpp"
#include "Utils.hpp"

#define REQUEST_BUFFER_SIZE 10240

class Request
{
	public:
		Request(std::string str);
		~Request();
	
		// /*Getters*/
		std::string getHeaderByName(const std::string &key) const
		{
			std::map<std::string, std::string>::const_iterator it = _headersmap.find(key);
			if (it != _headersmap.end())
				return it->second;
			return "";
		}
		int getContCRFL() { return this->_count_crlf;}
		std::map<std::string, std::string> getHeadersMap() const { return this->_headersmap; };
		std::string getMethod() const { return this->_method; };
		std::string getHTTPVersion() const { return this->_httpversion; };
		std::string getUri() const { return this->_uri; };
		void setUri(std::string &uri) { 
			this->_uri = uri;
		};
		std::string getHost() const { return this->_host; };
		int getStatusCode() const { return this->_status_code; };
		std::string getPayload() const { return this->_payload; };
		std::string getBody() const { return this->_body; };
		bool getIsValid() const { return this->_isvalid; };
		std::string getUriparams() const { return this->_uriparams; };
		std::string getHostPort() const { return this->_hostport; };
		std::string getPort() const { return this->_port; };
		std::string getUriExtension() const { return this->_uriextension; };
		void create_map(std::string str);
		void ParseRequestLine(std::string line);
		void ParseHeader(std::string headerstr);
		void ParseBody(std::string bodystr);
		void setPayload(std::string payload) { _payload = payload; };
	private:
		std::string _method;
		std::string _uri;
		std::string _httpversion;
		std::string _host;
		std::string _hostport;
		std::string _port;
		std::string _body;
		int 		_status_code;
		std::string _payload;
		bool 		_isvalid;
		std::string _uriparams;
		std::string _uriextension;
		std::map<std::string, std::string> _headersmap;
		int _count_crlf;
};

std::ostream &operator<<(std::ostream &os, const Request &request);
std::ostream &operator<<(std::ostream &os, const std::map<std::string, std::string> &map);

#endif