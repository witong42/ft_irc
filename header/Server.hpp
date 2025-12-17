/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 14:05:14 by jegirard          #+#    #+#             */
/*   Updated: 2025/12/15 17:29:36 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstring>
#include <cstdlib>
#include <cctype>
#include <stdexcept>


class Server
{
private:
	int _port; //port utilisé par le serveur
	int _fd; // fichier de descripteur du socket
	const char *_password; // mot de passe du serveur
	
	bool check_port(const char *port); // vérifie la validité du port
	bool socket();	// crée le socket
	bool socketUnblock(); // met le socket en non-bloquant

public:
	Server(const char *port, const char *password);
	~Server();
	class InvalidPortException : public std::exception
	{
		virtual const char* what() const throw()
		{
			return "Invalid port number";
		}
	};
	Server(int port, const char *password);
	

	void start();
};