/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcozigon <gcozigon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 15:38:40 by lboulang          #+#    #+#             */
/*   Updated: 2024/05/29 18:07:22 by gcozigon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "algorithm"

std::string getIndexOfHTML(const std::vector<std::string>& files, std::string dirname) {
    std::stringstream html;

    html << "<!DOCTYPE html>\n<html>\n<head>\n    <title>Index of " << dirname << "</title>\n</head>\n<body>\n    <h1>Index of " << dirname << "</h1>\n    <table>\n";
    for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it) {
        html << "<tr><td><a href=\"" << *it << "\">" << *it << "</a></td></tr>\n";
    }
    html << "        <tr><th colspan=\"4\"><hr></th></tr>\n    </table>\n</body>\n</html>\n";
    return html.str();
}

std::vector<std::string> getDirectoryFiles(std::string dir) {
    DIR* DirStream;
    struct dirent* dirp;
    std::vector<std::string> files;

    DirStream = opendir(dir.c_str());
    if (!DirStream) {
        std::cout << "Error opening directory: " << dir << std::endl;
        return files;
    }

    while ((dirp = readdir(DirStream)) != NULL) {
        std::string filename(dirp->d_name);
        struct stat filestat;
        std::string fullpath = dir + "/" + filename;
        if (stat(fullpath.c_str(), &filestat) == 0) {
            if (S_ISDIR(filestat.st_mode)) {
                filename += "/";
            }
        }
        files.push_back(filename);
    }
    closedir(DirStream);
	std::sort(files.begin(), files.end());
    return files;
}

void Response::processAutoIndex()
{
    std::vector<std::string> Dir = getDirectoryFiles(_fullpath);
	std::string Body = getIndexOfHTML(Dir, _request.getUri());
	SetBody(Body);
    SetHeaders("Content-Type: text/html");
    SetHeaders("Content-Length: " + ToString(Body.size()));
	SetStatus(200);
}