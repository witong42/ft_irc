/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegirard <jegirard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/15 14:05:14 by jegirard          #+#    #+#             */
/*   Updated: 2025/12/15 14:42:43 by jegirard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include <cstring>
#include <cstdlib>
#include <cctype>

class Server
{
private:
    int _port;
    const char* _password;
    bool check_port(const char *port);
public:
    Server(int port, const char* password);
    ~Server();

    void start();
}
;