/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 10:02:16 by jegirard          #+#    #+#             */
/*   Updated: 2025/12/18 11:34:51 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#define MAX_EVENTS 10
#define BUFFER_SIZE 512

#include <iostream>
#include <cstring>
#include <cstdlib>
#include "Server.cpp"



#define DEFAULT "\033[0m"				// #fefcfcff
#define GREEN "\033[0;38;2;121;200;88m" // #3da10eff
#define RED "\033[0;38;2;255;0;0m"		// #ff0000ff
# define INTROC  "\033[44;38;2;121;200;88m"


int main(int argc, char *argv[])
{
	if (argc != 3 || !argv[1] || !argv[2]|| argv[2][0] == '\0')
	{
		std::cerr << RED << "Error: Invalid number of arguments." << DEFAULT << std::endl;
		std::cerr << "Usage: /ircserv <port> <password>" << std::endl;
		return 1;
	}
	Server srv = Server(std::atoi(argv[1]), argv[2]);
	srv.run();
	
	
	
	
	std::cout << INTROC << "Starting IRC Server on port " << argv[1] << " with password " << argv[2] << DEFAULT << std::endl;
	
	return 0;
}
