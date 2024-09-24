/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseConfFile.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboulang <lboulang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 17:51:18 by lboulang          #+#    #+#             */
/*   Updated: 2024/05/28 13:57:42 by lboulang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parsing.hpp"

void CheckBracketsNumber(std::ifstream &configFile) {
	std::cout << "[PARSING] - Checking Brackets Number... ";
	std::string line;
	int count = 0;
	while (std::getline(configFile, line))
	{
		for (size_t i = 0; i < line.size(); i++)
		{
			if (line[i] == '{')
				count++;
			else if (line[i] == '}')
				count--;
		}
	}
	if (count != 0)
		throw WebServ::Err("\nBrackets number mismatch in config file.\n");
	std::cout << GREEN << "OK" << RESET << std::endl;
	ResetIfStream(configFile);
}

void OpenServerBlock(std::string &line, int &bracketCount, int &line_count)
{
	if (line.find("{") != line.size() - 1)
		throw WebServ::Err("Line " + ToString(line_count) + " : Error: Server block opening must be the single instruction on the line and followed by a newline.");
	bracketCount++;
	line_count++;
}

void OutSideServerBlock(std::string &line, int &line_count)
{
	for (size_t i = 0; i < line.size(); i++)
		if (!std::isspace(line[i]))
            throw WebServ::Err("Line " + ToString(line_count) + " : Error: Non-empty line found outside of server blocks.");
}

std::vector<std::string> ExtractServerBlock(std::ifstream &configFile)
{
	std::cout << "[PARSING] - Extracting Servers Block... ";
	std::string line;
	std::vector<std::string> serverBlocks;
	std::string currentBlock;
	int bracketCount = 0;
	int line_count = 1;
	while (std::getline(configFile, line))
	{
		std::size_t PosOpenServBlock = line.find("server {");
		if (PosOpenServBlock == 0) {
            OpenServerBlock(line, bracketCount, line_count); continue; }//sorry la norme but i dont like If brackets
		if (bracketCount == 0)
			OutSideServerBlock(line, line_count); // Check If Outside ServBlock is empty line
		if (bracketCount > 0)
			CloseServerBlock(line, currentBlock, bracketCount, line_count, serverBlocks);
		line_count++;
	}
	ResetIfStream(configFile);
	std::cout << GREEN << "OK" << RESET << std::endl;
	if (serverBlocks.empty())
		throw WebServ::Err("Error: No Server Block Found");
	return serverBlocks;
}

void CloseServerBlock(std::string &line, std::string &currentBlock, int &bracketCount, int &line_count, std::vector<std::string> &serverBlocks)
{
	currentBlock += line + "\n";
	for (size_t i = 0; i < line.size(); i++) {
		// Open another block
		if (line[i] == '{') {
			if (i != line.size() - 1)
				throw WebServ::Err("Line " + ToString(line_count) + " : Error: Opening bracket must be followed by a newline.");
			bracketCount++;
		}
		//Close block
		if (line[i] == '}') {
			if (i != line.size() - 1)//If Closing bracket is not the last char of the line
				throw WebServ::Err("Line " + ToString(line_count) + " : Error: Closing bracket must be followed by a newline.");
			bracketCount--;
			if (bracketCount == 0) {//Closing Bracket Close The ServerBlock
				currentBlock.erase(currentBlock.size() - 2, 2);
				serverBlocks.push_back(currentBlock);
				currentBlock.clear();
				return ; 
			}
		}
	}
}


bool WebServ::parseConfig(std::ifstream &configFile) {
	std::cout << "[PARSING] - Parsing Config File..." << std::endl;
	try
	{
		std::string line;
		CheckBracketsNumber(configFile);
		std::vector<std::string> serverBlocks = ExtractServerBlock(configFile);
		int i = 0;
		for (std::vector<std::string>::iterator it = serverBlocks.begin(); it != serverBlocks.end(); ++it) {
			this->_configlist.push_back(CreateConfigFromServerBlock(*it, i++));
		}
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (false);
	}
	std::cout << "[PARSING] - Config File Parsed" << std::endl;
	return (true);
}