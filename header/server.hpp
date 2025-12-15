/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 14:05:14 by jegirard          #+#    #+#             */
/*   Updated: 2025/12/15 15:13:00 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstring>
#include <cstdlib>
#include <cctype>
#include <stdexcept>


class Server
{
private:
	int _port;
	const char *_password;
	bool check_port(const char *port);

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