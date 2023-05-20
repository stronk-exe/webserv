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

std::vector<int>	_get_ports( Parsing &_server )
{
    std::vector<int>    _ports;

	// std::cerr << "ports: " << _server.servers.size() << std::endl;
	for (size_t i=0; i < _server.servers.size(); i++)
		_ports.push_back(_server.servers[i].listen_port);
	return _ports;
}

void	_socket( Parsing &_server, Request *request, Response *response )
{
    int					_socket_fd;
    struct sockaddr_in	address;
    int					addrlen;
    int					default_port;
	std::vector<int>	_socket_fds;
	fd_set				_sockets;

    
	addrlen = sizeof(address);
	default_port = _get_ports(_server)[0];

	// Initializing the sockets
	for (size_t i=0; i < _server.servers.size(); i++)
	{
		// Creating a socket for each server
		if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			print_error("socket creation failed!");
		
		// Binding the sockets of each server
		memset((char *)&address, 0, sizeof(address));
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(_server.servers[i].listen_port);

		int on = 1;
		if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)
			print_error("port in use!");

		if ((bind(_socket_fd, (struct sockaddr *)&address, sizeof(address))) < 0)
			print_error("binding failed!");
		
		// Start listining..
		if ((listen(_socket_fd, 10)) < 0)
			print_error("listining failed!");
		
		_socket_fds.push_back(_socket_fd);

	}
	// std::cerr << "haa: " << default_port << std::endl;
	// (void)_server;

    // if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    // {
    //     std::cerr << "socket creation failed!" << std::endl;
    //     exit(1);
    // }

    // memset((char *)&address, 0, sizeof(address));
    // address.sin_family = AF_INET;
    // address.sin_addr.s_addr = INADDR_ANY;
	// // std::cerr << "ha7na hna " << _server.servers[0].listen_port << std::endl;
    // address.sin_port = htons(default_port);

    // if ((bind(_socket_fd, (struct sockaddr *)&address, sizeof(address))) < 0)
    // {
    //     std::cerr << "binding failed!" << std::endl;
    //     exit(1);
    // }

    // if ((listen(_socket_fd, 10)) < 0)
    // {
    //     std::cerr << "listining failed!" << std::endl;
    //     exit(1);
    // }

	// int server_socket = 1;
	// fd_set current_sockets, ready_sockets;
	FD_ZERO(&_sockets);
	FD_SET(_socket_fd, &_sockets);

    while (1)
    {
        std::cout << "listening ..." << std::endl;
		// ready_sockets = current_sockets;
		if (select(FD_SETSIZE, &_sockets, NULL, NULL, NULL) < 0)
			print_error("error in select");

		for (int i=0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &_sockets))
			{
				if (std::find( _socket_fds.begin(), _socket_fds.end(), i) != _socket_fds.end())
				{
					int coming_socket;
					if ((coming_socket = accept(_socket_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
						print_error("acception failed!");

					char buffer[99999] = {0};
					int data = read(coming_socket, buffer, 99999);
					if (data < 0)
                        print_error("empty data!");

					std::cerr << "~ buffer:\n" << buffer << std::endl;
                    // 3- Request:
                    Server _s;
					_request(_server, _s, request, response, buffer);
                    // _match_theServer(_server, request, _s);
                    // std::cerr << "sssserver: " << _s.name << std::endl;


                    // checking the method
                    if (request->method == "GET")
                        _get(response, request, _s);
                    else if (request->method == "POST")
                        _post(response, request, _s);
                    else if (request->method == "DELETE")
                        _delete();
                    else
                        response->status = 405;
                    // {
                    //     std::cerr << "405 Method Not Allowed" << std::endl;
                    //     exit(1);
                    // }

					// Response
        			_response(response, request);

					std::cout << "wssa3 ya kho response jat:" << std::endl;
					std::cout << "response content_length " << response->content_length << std::endl;
					std::cout << "response content_type " << response->content_type << std::endl;
					std::cout << "response status " << response->status << std::endl;
					std::cout << "response content_type " << response->content_type << std::endl;
					
                    // std::cerr << "uri:" << request->uri <<  "| path: "<< request->path<< " | root: " << request->root << std::endl;

					const char *s = generate_response_str(response);
					write(coming_socket, s, strlen(s));
					
					close(coming_socket);
				}
			}
		}
		//////////////////////////////////


        // exit(1);


        
        
		
        
    }
}
