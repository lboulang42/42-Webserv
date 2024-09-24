/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboulang <lboulang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 18:21:47 by lboulang          #+#    #+#             */
/*   Updated: 2024/05/24 13:52:57 by lboulang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#define TCP_MAX_PORT 65535

#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define HALFSEP "================"
#define SEP 	"================================================="

# include <climits>
# include <map>
# include <algorithm>
# include <vector>
# include <iostream>
# include <string>
# include <sstream>
# include <fstream>
# include <strings.h>
# include <unistd.h>//execve();//dup();//dup2();//pipe();//access();//select();//close();//read();//write();//chdir();//fork();
# include <sys/epoll.h>//epoll_create();//epoll_ctl();//epoll_wait();
# include <string.h>//strerror();
# include <sys/socket.h>//socketpair();//socket();//accept();//listen();//send();//recv();//setsockopt();//getsockname();//bind();//connect();
# include <sys/stat.h>//stat();
# include <fcntl.h>//fcntl();//open();
# include <arpa/inet.h>//htons();//htonl();//ntohs();//ntohl();
# include <poll.h>//poll();
# include <sys/types.h>
# include <signal.h>//kill();//signal();
# include <dirent.h>//opendir();//readdir();//closedir();
# include <netdb.h>//gai_strerror();//getaddrinfo();//freeaddrinfo();//getprotobyname();
# include <sys/wait.h>//waitpid();
# include <errno.h>//errno();
# include "stdlib.h"//EXIT_SUCCESS;//EXIT_FAILURE;


//Convert string to anything
template<typename T>
T FromString(const std::string& str)
{
	std::istringstream is(str);
	T t;
	is >> t;
	return t;
}

//Convert anything to string
template<typename T>
std::string ToString(const T& t)
{
	std::ostringstream os;
	os << t;
	return os.str();
}

std::vector<std::string> splitleo(const std::string& s, const std::string& delimiter);
std::ostream &operator<<(std::ostream &os, const std::map<std::string, std::string> &map);
// std::string GetFromMap(const std::map<std::string, std::string>& map, const std::string& key);
bool isStringInVector(const std::vector<std::string>& vec, const std::string& str);
bool isNumber(const std::string& s);

/*Checking Files/Dir*/
bool isFileReadable(const std::string &path);
bool isDirectoryWritable(const std::string &path);
bool isDirectoryReadable(const std::string &path);
bool isDirectory(const std::string &path);

void ResetIfStream(std::ifstream &stream);

std::ostream &operator<<(std::ostream &os, const std::vector<std::string> &vec);
#endif