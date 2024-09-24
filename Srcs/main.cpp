/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboulang <lboulang@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 16:04:58 by gcozigon          #+#    #+#             */
/*   Updated: 2024/05/28 13:19:49 by lboulang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GomarServer.hpp"
#include "Response.hpp"
#include "Webserv.hpp"
#include "Utils.hpp"

int main(int ac, char **av)
{
	initializeMaps();
	WebServ MainProgram;
	
	try {
		if (ac != 2)
			throw WebServ::Err("Usage: " + std::string(av[0]) + " [configuration file]");
		MainProgram.loadConfig(av[1]);
		MainProgram.initServer(true);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
