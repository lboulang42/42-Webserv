/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseBlocks.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboulang <lboulang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 17:52:41 by lboulang          #+#    #+#             */
/*   Updated: 2024/05/28 13:22:37 by lboulang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parsing.hpp"

std::string ExtractLocationUri(std::string &location){
    std::map<std::string, std::string>map;
    std::vector<std::string> words = splitleo(location, " ");
    CheckLocation(words[1]);
    //trim location block
    location.erase(0, location.find_first_of("{\n") + 2);
    location.erase(location.find_last_of("}"));
    return (words[1]);
}

/*Load The Location Block for the default server params into a map*/
std::map<std::string, std::string> ParseLocation(std::string location, std::string locationname) {
    std::stringstream ss(location);
    std::string line;
    std::map<std::string, std::string>map;
    for (int i = 1; std::getline(ss,line); i++){
        if (line.empty())
			continue;
		std::vector<std::string> words = splitleo(line, " ");
		if (words.size() != 2 && words.size() > 0) {
			if (words[0] == "CGI")
				ParseConfigCGI(words, map);
			else
				throw WebServ::Err("Line : "+ToString<int>(i)+"\nInstructions Should Be Formated as instruction_name instruction_value\nYou Have :" + line);
		}
		if (!isStringInVector(AcceptedInstructions, words[0]))
			throw WebServ::Err("In a serverblock, Line " + ToString<int>(i) + " : " + "Invalid Instruction : '" + words[0] + "'\n");
        map[words[0]] = words[1];
    }
    if (!map["upload"].empty() && map["upload"][map["upload"].length() - 1] != '/')
		map["upload"] += "/";
	if (!map["root"].empty() &&map["root"][map["root"].length() - 1] != '/')
		map["root"] += "/";
	map["location"] = locationname;
    return map;
}

Config WebServ::CreateConfigFromServerBlock(std::string &serverBlock, int i)
{
	std::vector<std::string> locationBlocks = ExtractLocations(serverBlock, i);
	std::map<std::string, std::string> DefaultLocation;
	std::cout << "[PARSING] - Mapping Locations From Server N°" << i << "... ";
	std::map<std::string, std::map<std::string, std::string> > ServerMap;
	for (std::vector<std::string>::iterator it = locationBlocks.begin(); it != locationBlocks.end(); ++it) {
		std::map<std::string, std::string> LocationMap;
		if (it == locationBlocks.begin()){
			LocationMap = ParseLocation(*it, "/");
			CheckDefaultLocationMap(LocationMap);
            CheckListen(LocationMap["listen"]);//prechecked
            if (!LocationMap["server_names"].empty())
				CheckServerNames(LocationMap["server_names"]);
			CheckErrorPages(LocationMap);
		}
		else {
            std::string location = ExtractLocationUri(*it);
            LocationMap = ParseLocation(*it, location);
			if (location[0] != '/')
				location = "/"+location;
			if (location[location.length() - 1] != '/')
				location += "/";
			LocationMap["location"] = location;
            CheckLocationMap(LocationMap);
		}
		ConfigCheckRoot(LocationMap["root"]);//must have been set
		if (!LocationMap["max_body"].empty())
			CheckMaxBody(LocationMap["max_body"]);
		if (!LocationMap["allowed_methods"].empty())
			CheckMethods(LocationMap["allowed_methods"]);
		if (!LocationMap["autoindex"].empty())
			CheckAutoIndex(LocationMap["autoindex"]);
		if (!LocationMap["defaultpage"].empty())
			CheckDefaultPage(LocationMap["root"], LocationMap["defaultpage"]);
		if (!LocationMap["upload"].empty())
			CheckUpload(LocationMap["upload"]);
		ServerMap[LocationMap["location"]] = LocationMap;
	}
	std::cout << GREEN << "OK" << RESET << std::endl;
	Config config(ServerMap);
	return config;
}

/*Separate ServerBlock into Locationblocks to map routes*/
std::vector<std::string> ExtractLocations(const std::string &serverBlock, int i) {
	std::cout << "[PARSING] - Extracting Locations Block From Server N°" << i << "... ";
    
	std::stringstream ss(serverBlock);
	std::vector<std::string> locationBlocks;
    std::string defaultlocationblock;
	std::string currentBlock;
	std::string line;
	int bracketCount = 0;
    
	while (std::getline(ss, line)) {
		line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));//Get Rid Of WhiteSpaces at the beginning of the line
		std::size_t PosOpenLocBlock = line.find("location ");
		if (PosOpenLocBlock == 0) { // Check if "location " is at the start of the line
			std::vector<std::string>line_words = splitleo(line, " ");
			if (line_words.size() != 3 || line_words[1].empty() || line_words[2] != "{")
				throw WebServ::Err("\nError: Location Block must be formated as :\nlocation /path {\n\tinstruction_name instruction_value\n}\n");
			bracketCount++;
			currentBlock += line + "\n";
			continue;
		}
		if (bracketCount > 0) {
			currentBlock += line + "\n";
			if (line.find("}") != std::string::npos) {
				bracketCount--;
				if (bracketCount == 0) {
					locationBlocks.push_back(currentBlock);
					currentBlock.clear();
				}
			}
		} 
        else
			defaultlocationblock += line + "\n";
	}
    
	if (defaultlocationblock.empty())
		throw WebServ::Err("\nError: No Default Location Block Found");
	if (!defaultlocationblock.empty() && defaultlocationblock.find_first_not_of(" \t\n\r\f\v") != std::string::npos)
		locationBlocks.insert(locationBlocks.begin(), defaultlocationblock);//Default Location Block at the beginning
	if (locationBlocks.empty())
		throw WebServ::Err("\nEmpty ServerBlock");
        
	std::cout << GREEN << "OK" << RESET << std::endl;
	return locationBlocks;
}

