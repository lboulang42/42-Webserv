/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcozigon <gcozigon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:16:44 by lboulang          #+#    #+#             */
/*   Updated: 2024/05/29 19:49:45 by gcozigon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "algorithm"
#include "Request.hpp"
#include "Config.hpp"

std::string Response::GetRestOfUri(std::string rootdir, std::string uri)
{
	std::string restofuri = uri;
	if (uri.find(rootdir) != std::string::npos)
		restofuri = uri.substr(uri.find(rootdir) + rootdir.size());
	if (restofuri[0] == '/')
		restofuri.erase(0, 1);
	return restofuri;
}

Response::Response(Request req, Config conf) : 
	_request(req), 
	_config(conf), 
	_headers(), 
	_status_message(""), 
	_fullpath(""), 
	_body(""), 
	_hasbeenredirect(false), 
	_status_code(0),
	_rootdirectory(_config.GetRootDirFromConfig(_request.getUri())),
	_restofuri(GetRestOfUri(_config.GetLocationFromConfig(_request.getUri()), _request.getUri())),
	_maplocationkey(_config.GetLocationFromConfig(_request.getUri())),
	_sortedlocationlist(_config.GetSortedLocationList(_request.getUri()))
{
	std::string tmp = _rootdirectory+_restofuri;
	if (isDirectory(tmp) && tmp[tmp.size() - 1] != '/'){
		std::string requri = _request.getUri() + "/";
		this->_request.setUri(requri);
		_rootdirectory = _config.GetRootDirFromConfig(_request.getUri());
		_restofuri = GetRestOfUri(_config.GetLocationFromConfig(_request.getUri()), _request.getUri());
		_maplocationkey = _config.GetLocationFromConfig(_request.getUri());
		_sortedlocationlist = _config.GetSortedLocationList(_request.getUri());
	}
}

void Response::processBody()
{
	HandleMIMESTypes();
	std::ifstream file(this->_fullpath.c_str());
	std::stringstream ss;
	ss << file.rdbuf();
	SetBody(ss.str());
}

bool Response::HasRedirect()
{
	std::string RedirectLink = _config.GetRedirect(_maplocationkey);
	if (RedirectLink != "")
	{
		this->_hasbeenredirect = true;
		SetStatus(302);
		if (RedirectLink.find("http://") != std::string::npos && RedirectLink.find("https://") != std::string::npos)
			RedirectLink = "/" + RedirectLink;
		SetHeaders("Location: "  + RedirectLink);
		SetHeaders("Content-Length: 0");
		return true;
	}
	return false;
}

// redirect to default page for this url directory
void Response::processDefaultPage()
{
	SetHeaders("Location: " + _config.GetDefaultPageNew(_maplocationkey));
	SetHeaders("Content-Length: 0");
	SetStatus(301);//redirect to defaultpage (mooved permanently)
}

bool Response::IsDirectoryUriNew() {
	if (!isDirectory(_fullpath))
		return false;
	if (_config.GetDefaultPageNew(_maplocationkey) != "")
		return (processDefaultPage(), true);
	if (_config.GetAutoIndexFromConfig(_maplocationkey) == "on")
		return (processAutoIndex(), true);
	ProcessError(403);//Calling to a directory if no autoindex/no default page return a 403
	return true;
}

bool Response::ValidContentLength()
{
	if (_request.getMethod() != "POST") // if not post, no need to check content length
		return true;
	
	std::cout << "Content-Length: " << _request.getHeaderByName("Content-Length") << std::endl;
	std::cout << "Payload: " << _request.getPayload().size() << std::endl;
	// for (int i = 0; i <= (_request.getContCRFL() - 1); i++)
	// 	_request.setPayload(_request.getPayload().append("\r\n\r\n"));
	std::cout << "AFTER PAYLOAD: " << std::endl;
		std::cout << "Content-Length: " << _request.getHeaderByName("Content-Length") << std::endl;
	std::cout << "Payload: " << _request.getPayload().size() << std::endl;
	if (_request.getHeaderByName("Content-Length").empty() && _request.getPayload().size() > 0)
		return (ProcessError(411), false);
	if (!_request.getHeaderByName("Content-Length").empty() && (FromString<size_t>(_request.getHeaderByName("Content-Length")) != _request.getPayload().size())){
		std::cout << _request.getBody().size() << std::endl;
		return (ProcessError(411), false);
	}
		
	return true;
}

bool Response::CheckVersionHTTP()
{
	if (_request.getHTTPVersion() != "HTTP/1.1" && _request.getHTTPVersion() != "HTTP/1.0")
	{
		ProcessError(505);
		return false;
	}
	return true;
}

bool Response::checkmethod(std::string method)
{
	std::vector<std::string> allowed = _config.GetAllowedMethodsFromConfig(_request.getUri());
	for (std::vector<std::string>::iterator it = allowed.begin(); it != allowed.end(); ++it)
		if (*it == method)
			return true;
	return false;
}

void Response::ProcessError(int statuscode) {
	SetStatus(statuscode);
	if (_config.GetErrorPage(_request.getUri(), statuscode) != ""){
		SetFullPath("", _config.GetErrorPage(_request.getUri(), statuscode));
		processBody();
	}
	if (statuscode == 405){
		std::vector<std::string> allowedvec = _config.GetAllowedMethodsFromConfig(_request.getUri());
		std::string allowed;
		for (std::vector<std::string>::iterator it = allowedvec.begin(); it != allowedvec.end(); ++it){
			if (it != allowedvec.begin())
				allowed += ", ";
			allowed += *it;
		}
		SetHeaders("Allow: " + allowed);
	}
	SetHeaders("Content-Length: " + ToString(_body.size()));
}

bool Response::IsAllowedMethod()
{
	if (!checkmethod(this->_request.getMethod()))
	{
		ProcessError(405);
		return false;
	}
	return true;
}

bool Response::check_max_body()
{
	if (_request.getPayload().size() > FromString<unsigned long>(_config.GetMaxBodyFromConfig(_request.getUri())))
	{
		ProcessError(413);
		return false;
	}
	return true;
}

void Response::processDelete(){
	if (std::remove(_fullpath.c_str()) == 0){
		SetStatus(204);

		SetHeaders("Content-Length: 0");
	}
	else
		ProcessError(404);
}

void Response::processRequest()
{
	SetHeaders("Server: Webserv/1.0");
	SetFullPath(_rootdirectory, _restofuri);
	if (_request.getIsValid() == false)
	{
		ProcessError(400);
		return;
	}
	if (access(_fullpath.c_str(), F_OK) == -1)
	{
		ProcessError(404);
		return;
	}
	if (!IsAllowedMethod())
		return;
	if (!HasValidUri())
		return;
	if (CheckVersionHTTP() == false)
		return ;
	if (check_max_body() == false)
		return ;
	if (ValidContentLength() == false)
		return ;
	if (HasRedirect())
		return;
	if (IsDirectoryUriNew())
		return;

    if (can_exec_cgi()){
    	processCGI();
		return ; 
	}
	if (_request.getMethod() == "DELETE")
	{
		processDelete();
		return;
	}

	SetStatus(200);
	processBody();
	SetHeaders("Content-Length: " + ToString(_body.size()));
}

std::ostream &operator<<(std::ostream &os, const Response &Response)
{
	os << "_status_code : '" << GREEN << Response.GetStatusCode() << RESET << "'\n"
	   << "_status_message : '" << GREEN << Response.GetStatusMessage() << RESET << "'\n"
	   << "_fullpath : '" << GREEN << Response.GetFullPath() << RESET << "'\n"
	   << "_body : '" << GREEN << Response.GetBody() << RESET << "'\n";
	   os << "_rootdirectory" << GREEN << Response.Getrootdirectory() << RESET << "'\n";
	   os << "_restofuri" << GREEN << Response.Getrestofuri() << RESET << "'\n";
	   os << "_maplocationkey " << GREEN << Response.GetMapLocationKey() << RESET << "'\n";
	std::vector<std::string> headers = Response.GetHeaders();
	for (std::vector<std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
		os << "_headers : '" << GREEN << *it << RESET << "'\n";
	return os;
}

void Response::HandleMIMESTypes()
{
	std::string FileExtension = this->_request.getUriExtension();
	if (FileExtension == "")
		return ((void)SetHeaders("Content-Type: text/html"));// if no file extension -> application/octet-stream
	if (MIMESTypeMap.find(FileExtension) != MIMESTypeMap.end())
		SetHeaders("Content-type: " + MIMESTypeMap[FileExtension]); // if supported extension, set the header
	else
		SetHeaders("Content-Type: text/html"); // if not supported extension -> application/octet-stream
}

bool Response::HasValidUri()
{
	if (!IsFileFound()) {
		return (ProcessError(404), false);
	}
	return true;
}

bool Response::IsFileFound()
{
	if (access(this->_fullpath.c_str(), R_OK) == 0)
		return true;
	return false;
}

std::string Response::GetHeadersString()
{
	std::string headers;
	for (std::vector<std::string>::iterator it = _headers.begin(); it != _headers.end(); ++it)
		headers += *it + "\n";
	return headers;
};

std::string Response::GetStatusLine()
{
	return "HTTP/1.1 " + ToString(_status_code) + " " + _status_message + "\r\n";
};
