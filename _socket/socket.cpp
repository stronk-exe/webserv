/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-asri <ael-asri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/11 11:14:33 by ael-asri          #+#    #+#             */
/*   Updated: 2023/05/11 11:24:04 by ael-asri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp"

const char *generate_response_str(Response *_response)
{
	std::string s = "HTTP/1.1 "+ std::to_string(_response->status)+" "+_response->status_message+\
					"\nContent-Type: "+_response->content_type+\
					"\nContent-Length: "+std::to_string(_response->content_length)+\
					"\n\n"+_response->body;
	return s.c_str();
}

void _socket( Parsing &_server, Request *request, Response *response )
{
    int _socket_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // (void)_server;

    if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "socket creation failed!" << std::endl;
        exit(1);
    }

    memset((char *)&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
	std::cerr << "ha7na hna " << _server.servers[0].listen_port << std::endl;
    int  default_port = 80;
    address.sin_port = htons(default_port);

    if ((bind(_socket_fd, (struct sockaddr *)&address, sizeof(address))) < 0)
    {
        std::cerr << "binding failed!" << std::endl;
        exit(1);
    }

    if ((listen(_socket_fd, 10)) < 0)
    {
        std::cerr << "listining failed!" << std::endl;
        exit(1);
    }

	// int server_socket = 1;
	fd_set current_sockets, ready_sockets;
	FD_ZERO(&current_sockets);
	FD_SET(_socket_fd, &current_sockets);

    while (1)
    {
        std::cout << "listening ..." << std::endl;
		ready_sockets = current_sockets;
		if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
			print_error("error in select");
		
        int coming_socket;
        if ((coming_socket = accept(_socket_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
        {
            std::cerr << "acception failed!" << std::endl;
            exit(1);
        }

		for (int i=0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &ready_sockets))
			{
				if (i == _socket_fd)
				{
					char buffer[99999] = {0};
					int data = read(coming_socket, buffer, 99999);
					if (data < 0)
					{
						std::cerr << "empty data!" << std::endl;
						exit(1);
					}
					std::cout << "buffer: " << buffer  << "~" << std::endl;

					// 3- Request:

                    Server _s;
                    // _match_theServer(_server, request, _s);
                    // std::cerr << "sssserver: " << _s.name << std::endl;

					_request(_server, _s, request, response, buffer);

                    // checking the method
                    if (request->method == "GET")
                        _get(response, request, _s);
                    // else if (request->method == "POST")
                    //     _post(response, request, _server);
                    // else if (request->method == "DELETE")
                    //     _delete();
                    else
                        response->status = 405;
                    // {
                    //     std::cerr << "405 Method Not Allowed" << std::endl;
                    //     exit(1);
                    // }

					// Response
        			_response(response, request);

					const char *s = generate_response_str(response);
					write(coming_socket, s, strlen(s));
				}
			}
		}
		//////////////////////////////////


        // exit(1);


        
        // // std::cout << "wssa3 ya kho response jat:" << std::endl;
        // // std::cout << "response content_length " << response->content_length << std::endl;
        // // std::cout << "response content_type " << response->content_type << std::endl;
        // // std::cout << "response status " << response->status << std::endl;
        // // std::cout << "response content_type " << response->content_type << std::endl;
        
		
        close(coming_socket);
    }
}
