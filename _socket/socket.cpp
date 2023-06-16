/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-jao <mait-jao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/11 11:14:33 by ael-asri          #+#    #+#             */
/*   Updated: 2023/06/11 20:39:05 by mait-jao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp"

void generate_response_str(Response *_response, int coming_socket)
{
	std::string s = "HTTP/1.1 "+ std::to_string(_response->status)+" "+_response->status_message+\
					"\nContent-Type: "+_response->content_type+\
					"\nContent-Length: "+std::to_string(_response->content_length)+\
					"\n\n"+_response->body;
					
	write(coming_socket, s.c_str(), s.size());
}
std::vector<int>	_get_ports( Parsing &_server )
{
    std::vector<int>    _ports;

	for (size_t i=0; i < _server.servers.size(); i++)
		_ports.push_back(_server.servers[i].listen_port);
	return _ports;
}

void	_init_l3alam( Request *_request, Response *_response)
{
	// Request
	_request->body = "";
	_request->is_method_allowed = 0;

	// Response
	_response->status = 0;
	_response->content_length = 0;
	_response->body = "";

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
	FD_ZERO(&_sockets);
	FD_SET(_socket_fd, &_sockets);

    while (1)
    {
        std::cout << "listening ..." << std::endl;
		// ready_sockets = current_sockets;
		if (select(FD_SETSIZE, &_sockets, NULL, NULL, NULL) < 0)
			print_error("error in select");

		_init_l3alam(request, response);
		for (int i=0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &_sockets))
			{
				if (std::find( _socket_fds.begin(), _socket_fds.end(), i) != _socket_fds.end())
				{
					int coming_socket;
					if ((coming_socket = accept(_socket_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
						print_error("acception failed!");

					request->fd = coming_socket;
					char buffer[999999] = {0};
					int data = read(coming_socket, buffer, 999999);
					if (data < 0)
                        print_error("empty data!");

					// std::ofstream _hmida("uploads/hmida.txt");
					std::string _test_buffer;
					for (int i=0; i<data; i++)
						_test_buffer += buffer[i];
				
                    // 3- Request:
                    Server _s;
					_request(_server, _s, request, response, _test_buffer);


                    // checking the method
					if (request->is_method_allowed)
					{
						if (request->method == "GET")
							_get(response, request, _s);
						else if (request->method == "POST")
							_post(response, request, _s);
						else if (request->method == "DELETE")
							_delete(response, request, _s);
					}
                    else
                        response->status = 405;
				
					// Response
        			_response(response, request);
					generate_response_str(response, coming_socket);
					
					close(coming_socket);
				}
			}
		}
    }
}

// void	_socket( Parsing &_server, Request *request, Response *response )
// {
//     int					_socket_fd;
//     struct sockaddr_in	address;
//     int					addrlen;
//     int					default_port;
// 	std::vector<int>	_socket_fds, accepted_fds;
// 	fd_set				_sockets;

    
// 	addrlen = sizeof(address);
// 	default_port = _get_ports(_server)[0];

// 	// Initializing the sockets
// 	for (size_t i=0; i < _server.servers.size(); i++)
// 	{
// 		// Creating a socket for each server
// 		if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
// 			print_error("socket creation failed!");
		
// 		// Binding the sockets of each server
// 		memset((char *)&address, 0, sizeof(address));
// 		address.sin_family = AF_INET;
// 		address.sin_addr.s_addr = INADDR_ANY;
// 		address.sin_port = htons(_server.servers[i].listen_port);

// 		int on = 1;
// 		if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)
// 			print_error("port in use!");

// 		if ((bind(_socket_fd, (struct sockaddr *)&address, sizeof(address))) < 0)
// 			print_error("binding failed!");
		
// 		// Start listining..
// 		if ((listen(_socket_fd, 10)) < 0)
// 			print_error("listining failed!");
		
// 		_socket_fds.push_back(_socket_fd);
// 		FD_ZERO(&_sockets);
// 		FD_SET(_socket_fd, &_sockets);
// 	}
	
// 	int x=0;
//     while (1)
//     {
//         std::cout << "listening ..." << std::endl;
// 		// ready_sockets = current_sockets;
// 		if (select(FD_SETSIZE, &_sockets, NULL, NULL, NULL) < 0)
// 			print_error("error in select");
// 		// std::cerr << "vvvvvvv" << _socket_fds[0] << std::endl;

// 		_init_l3alam(request, response);
// 		for (int i=0; i < FD_SETSIZE; i++)
// 		{
// 			if (FD_ISSET(_socket_fds[i], &_sockets))
// 			{
// 				if (std::find( _socket_fds.begin(), _socket_fds.end(), _socket_fds[i]) != _socket_fds.end())
// 				{
// 					int coming_socket;
// 					if ((coming_socket = accept(_socket_fds[i], (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
// 						print_error("acception failed!");
// 					FD_SET(coming_socket, &_sockets);
// 					// accepted_fds.push_back(coming_socket);
// 					std::cerr << "hillownnn-" << x << std::endl;
// 				// }
// 				// else if (std::find( accepted_fds.begin(), accepted_fds.end(), i) != accepted_fds.end())
// 				// {

// 					// request->fd = coming_socket;
// 					char buffer[9999] = {0};
// 					int data = read(_socket_fds[i], buffer, 9999);
// 					if (data < 0)
//                         print_error("empty data!");
// 					std::cerr << "read: " << data << std::endl;

// 					// std::ofstream _hmida("uploads/hmida.txt");
// 					std::string _test_buffer;
// 					for (int i=0; i<data; i++)
// 						_test_buffer += buffer[i];
					
// 					// std::cerr << "buffer: " << _test_buffer << "\nvector: " << _socket_fds.size() << "\ni: " << i << std::endl;
				
//                     // 3- Request:
//                     Server _s;
// 					_request(_server, _s, request, response, _test_buffer);


//                     // checking the method
//                     if (request->method == "GET")
//                         _get(response, request, _s);
//                     else if (request->method == "POST")
//                         _post(response, request, _s);
//                     else if (request->method == "DELETE")
//                         _delete(response, request, _s);
//                     else
//                         response->status = 405;
				
// 					// Response
//         			_response(response, request);
					
// 					const char *s = generate_response_str(response);
// 					write(_socket_fds[i], s, strlen(s));
					
// 					// close(i);

// 					// FD_CLR(i, &_sockets);
// 					// _socket_fds.pop_back();
// 					std::cerr << "hillow-" << x << std::endl;
// 				// }
// 				// else {
// 					close(_socket_fds[i]);
// 				}
// 				x+=1;
// 			}
// 		}
//     }
// }
