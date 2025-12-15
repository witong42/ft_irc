/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 10:02:16 by jegirard          #+#    #+#             */
/*   Updated: 2025/12/15 13:13:16 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#define DEFAULT "\033[0m"				// #fefcfcff
#define GREEN "\033[0;38;2;121;200;88m" // #3da10eff
#define RED "\033[0;38;2;255;0;0m"		// #ff0000ff
# define INTROC  "\033[44;38;2;121;200;88m"



int main(int argc, char *argv[]
)
{
	if (argc != 3 || !argv[1] || !argv[2]|| argv[2][0] == '\0')
	{
		std::cerr << RED << "Error: Invalid number of arguments." << DEFAULT << std::endl;
		std::cerr << "Usage: ";
		std::cerr << "/ircserv <port> <password>" << std::endl;
		return 1;
	}
	std::cout << INTROC << "Starting IRC Server on port " << argv[1] << " with password " << argv[2] << DEFAULT << std::endl;
	
	return 0;
}
