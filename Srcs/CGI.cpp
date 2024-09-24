/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboulang <lboulang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 17:23:58 by lboulang          #+#    #+#             */
/*   Updated: 2024/05/29 20:26:21 by lboulang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "algorithm"
#include "Request.hpp"
#include "Config.hpp"
#include <sys/time.h>

bool Response::can_exec_cgi()
{
    std::string extention = _config.GetCGIExtFromConfig(_request.getUri());
    std::vector<std::string> tab = splitleo(extention, "|");
    for (std::vector<std::string>::iterator it = tab.begin(); it != tab.end(); ++it){
        if (("."+_request.getUriExtension()) == *it)
            return true;
	}
    return false;
}

void Response::processCGI()
{
	if (!execCGI())
		return ;
	SetHeaders("Content-Length: " + ToString(_body.size()));
	if (this->_status_code == 0)
		SetStatus(200);	
}

std::string GetCGIOutput(int PipeOut)
{
	std::string output;
	char buffer[1024];
	ssize_t num_bytes_received;
	while ((num_bytes_received = read(PipeOut, buffer, sizeof(buffer) - 1)) > 0)
	{
		buffer[num_bytes_received] = '\0';
		output.append(buffer);
	}
	return output;
}

void freeCharPtrArray(char** arr) {
    if (arr != NULL) {
        for (int i = 0; arr[i] != NULL; ++i) {
            free(arr[i]);
        }
        delete[] arr;
    }
}

bool Response::execCGI()
{
	struct timeval start_time, current_time;
	gettimeofday(&start_time, NULL);
	std::string extention = _config.GetCGIExtFromConfig(_request.getUri());
	std::string cgiPath = _config.GetCGIPathFromConfig(_request.getUri());
	char **envp = setCGIenv();
	int pipefd[2];
	int pipefdout[2];
	if (pipe(pipefd) == -1 )
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	if (pipe(pipefdout) == -1){
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	pid_t pid = fork();
	if (pid == -1)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if (pid == 0)
	{
		close(pipefd[1]);
		dup2(pipefd[0], STDIN_FILENO);
		close(pipefdout[0]);
		dup2(pipefdout[1], STDOUT_FILENO);
		close(pipefdout[1]);
		char *argv[] = {const_cast<char*>(cgiPath.c_str()), NULL};
		execve(argv[0], argv, envp);
		perror("execve");
		freeCharPtrArray(envp);
		exit(EXIT_FAILURE);
	}
	else
	{
		close(pipefd[0]);
		close(pipefdout[1]);
		write(pipefd[1], this->_request.getPayload().c_str(), this->_request.getPayload().size());
		close(pipefd[1]);
		
		// Set up the timeout
    	while (true)
    	{
			//Check For Timeout
			gettimeofday(&current_time, NULL);
			if (current_time.tv_sec - start_time.tv_sec > 1)
			{
				kill(pid, SIGKILL);
				close(pipefdout[0]);
				ProcessError(500);
				freeCharPtrArray(envp);
				return false; 			
			}

			int status;
			pid_t result = waitpid(pid, &status, WNOHANG);
			if (result > 0)
			{
				std::string output = GetCGIOutput(pipefdout[0]);
				close(pipefdout[0]);
				parsePhpCgiOut(output);
				freeCharPtrArray(envp);
				break;
			}
    	}
	}
	return true;
}



void Response::parsePhpCgiOut(std::string output)
{
	size_t pos = output.find("\r\n\r\n");
	std::string::size_type pos2 = 0;
	std::string::size_type prev = 0;
	bool headersEnded = false;

	while ((pos2 = output.find("\r\n", prev)) != std::string::npos) {
		std::string line = output.substr(prev, pos2 - prev);
		if (line.empty())
		{
			headersEnded = true;
		}
		else if (!headersEnded)
		{
			if (line.find("Status:") != std::string::npos)
				SetStatus(FromString<int>(line.substr(8, 3)));
			else
				SetHeaders(line);
		}
		prev = pos2 + 2; // +2 to skip "\r\n"
	}
	SetBody(output.substr(pos + 4));
}


std::string setCGIenvUplDir(std::string uploaddir){
	std::string upldir;

	char cwd[1024];
	if (uploaddir.empty() && getcwd(cwd, sizeof(cwd)) != NULL)
		return (std::string(cwd)+'/');
	if (uploaddir[0] != '/' && getcwd(cwd, sizeof(cwd)) != NULL)
		return (std::string(cwd) + '/' + uploaddir);
	return uploaddir;
}

char **Response::setCGIenv()
{
	std::map<std::string, std::string> mapenv;
	mapenv["GATEWAY_INTERFACE"] = "CGI/1.1";
	mapenv["SERVER_PROTOCOL"] = "HTTP/1.1";
	mapenv["REDIRECT_STATUS"] = "200";
	mapenv["HTTP_HOST"] = _request.getHost();//change
	mapenv["REQUEST_METHOD"] = _request.getMethod();
	mapenv["SCRIPT_FILENAME"] = _fullpath;
	mapenv["SERVER_SOFTWARE"] = "WebServ1.1 sortez moi de la svp";
	mapenv["SERVER_PORT"] = _request.getPort();
	mapenv["SERVER_NAME"] = _request.getHost();//change
	mapenv["PATH_INFO"] = _request.getUri();
	mapenv["PATH_TRANSLATED"] = _fullpath;
	mapenv["SCRIPT_NAME"] = _fullpath;
	mapenv["CONTENT_LENGTH"] = _request.getHeaderByName("Content-Length");
	mapenv["CONTENT_TYPE"] = _request.getHeaderByName("Content-Type");
	mapenv["QUERY_STRING"] = _request.getUriparams();
	mapenv["REQUEST_URI"] = _request.getUri();//asked by tester
	mapenv["UPLOAD_DIR"] = setCGIenvUplDir(_config.GetUploadFromConfig(_request.getUri()));
	
	char **env = new char *[mapenv.size() + 1];
	int i = 0;
	for (std::map<std::string, std::string>::iterator it = mapenv.begin(); it != mapenv.end(); ++it)
	{
		std::string envvar = it->first + "=" + it->second;
		env[i] = strdup(envvar.c_str());
		i++;
	}
	env[i] = NULL;
	return env;
}
