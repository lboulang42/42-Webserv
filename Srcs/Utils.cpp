/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboulang <lboulang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 22:22:19 by lboulang          #+#    #+#             */
/*   Updated: 2024/05/28 13:07:10 by lboulang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

std::ostream &operator<<(std::ostream &os, const std::map<std::string, std::string> &map){
	os << "Printing Map string:string" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = map.begin(); it != map.end(); it++)
		os << "Key [" << it->first <<"]" << " => " << it->second << std::endl;
	return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<std::string> &vec){
	os << "Printing Vector string" << std::endl;
	int i = 0;
	for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); it++){
		os << "vec["<<i<<"] : " << BLUE << *it << RESET << std::endl;
	}
	return os;
}

bool isStringInVector(const std::vector<std::string>& vec, const std::string& str) {
    return std::find(vec.begin(), vec.end(), str) != vec.end();
}

bool isNumber(const std::string& s)
{
	std::istringstream iss(s);
	long num;
	iss >> num;
	return iss.eof() && !iss.fail(); 
}

bool isDirectoryReadable(const std::string &path)
{
	struct stat info;
	if (stat(path.c_str(), &info) != 0)
		return false;
	if (info.st_mode & S_IFDIR)
		if (!access(path.c_str(), R_OK))
			return true;
	return false;
}

bool isDirectory(const std::string &path){
	struct stat info;
	if (stat(path.c_str(), &info) != 0)
		return false;
	if (info.st_mode & S_IFDIR)
		return true;
	return false;
}

bool isDirectoryWritable(const std::string &path) {
	struct stat info;
	if (stat(path.c_str(), &info) != 0)
		return false;
	if (info.st_mode & S_IFDIR)
		if (!access(path.c_str(), W_OK))
			return true;
	return false;
}

bool isFileReadable(const std::string &path) {
	struct stat info;
	if (stat(path.c_str(), &info) != 0)
		return false;
	if (info.st_mode & S_IFREG){
		if (!access(path.c_str(), R_OK))
			return true;
	}
	return false;
}

void ResetIfStream(std::ifstream &stream)
{
	stream.clear();
	stream.seekg(0, std::ios::beg);
}