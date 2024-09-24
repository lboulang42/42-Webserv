/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboulang <lboulang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 12:50:04 by lboulang          #+#    #+#             */
/*   Updated: 2024/05/28 15:39:40 by lboulang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include "Config.hpp"
#include "Request.hpp"
#include "Webserv.hpp"
#include "Utils.hpp"

/*
https://datatracker.ietf.org/doc/html/rfc2616#section-6
Status-Line               ; Section 6.1
						*(( general-header        ; Section 4.5
						| response-header        ; Section 6.2
						| entity-header ) CRLF)  ; Section 7.1
						CRLF
						[ message-body ]          ; Section 7.2
*/

std::vector<std::string> getDirectoryFiles(std::string dir);
std::string getIndexOfHTML(const std::vector<std::string> &files, std::string dirname);

class Response
{
	public:
		/*Constructors/Destructors*/
		Response(Request req, Config conf);
		~Response(){};
		void processDelete();
		/*Getters*/
		bool can_exec_cgi();
		Request	GetRequest() const { return (_request); };
		Config	GetConfig() const { return (_config); };
		std::vector<std::string> GetHeaders() const { return (_headers); };
		std::string GetStatusMessage() const { return (_status_message); };
		// std::string GetContentType() const { return (_contenttype); };
		std::string GetFullPath() const { return (_fullpath); };
		std::string GetBody() const { return (_body); };
		std::string GetStatusLine();
		std::string GetMapLocationKey() const { return (_maplocationkey); };
		std::string GetHeadersString();
		int 		GetStatusCode() const { return (_status_code); };
		std::string Getrootdirectory() const { return (_rootdirectory); };
		std::string Getrestofuri() const { return (_restofuri); };
		/*Setters*/
		void SetStatus(int status_code) { _status_code = status_code; _status_message = StatusMap.at(status_code); };
		void SetHeaders(std::string header) { _headers.push_back(header); };
		void SetBody(std::string body) { this->_body = body; };
		void SetFullPath(std::string rootdirectory, std::string uri) {
			if (!uri.empty() && uri[0] == '/')
				uri.erase(0, 1);
			_fullpath = rootdirectory + uri; 
			// if (_fullpath.find("//") != std::string::npos)
				// _fullpath = _fullpath.substr(0, _fullpath.find("//")) + _fullpath.substr(_fullpath.find("//") + 1);
		}; // NEED CHANGE


		bool IsDirectoryUriNew();
		/*Response Methods*/
		bool HasRedirect();
		bool HasValidUri();
		void processRequest();
		void processBody();
		void processDefaultPage();
		void HandleMIMESTypes();
		void ProcessError(int statuscode);
		bool ValidContentLength();
		bool CheckVersionHTTP();
		bool checkmethod(std::string method);
		bool IsAllowedMethod();
		bool IsFileFound();
		bool check_max_body();
		// CGI
		void parsePhpCgiOut(std::string out);
		char **setCGIenv();
		void processCGI();
		bool execCGI();
	
		//AutoIndex	
		void processAutoIndex();
		std::string GetRestOfUri(std::string rootdir, std::string uri);
		//Send Request
		std::string Send() { return (GetStatusLine() + GetHeadersString() + "\r\n" + GetBody()); };

	private:
		Request 					_request;
		Config 						_config;
		std::vector<std::string>	_headers;
		std::string					_status_message;
		// std::string					_contenttype;
		std::string					_fullpath;
		std::string					_body;
		bool						_hasbeenredirect;
		int							_status_code;
		std::string 				_rootdirectory;//to get path
		std::string 				_restofuri;
		std::string _maplocationkey;
		std::vector<std::string> _sortedlocationlist;
};

std::ostream &operator<<(std::ostream &os, const Response &response);

#endif