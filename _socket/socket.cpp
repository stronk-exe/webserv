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

std::string	generate_response_str(Response *_response)
{
	return "HTTP/1.1 "+ std::to_string(_response->status)+" "+_response->status_message+\
					"\nContent-Type: "+_response->content_type+\
					"\nContent-Length: "+std::to_string(_response->content_length)+\
					"\n\n"+_response->body;
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
	_request->path = "";
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
	int					_wr = 0;
	std::vector<int>	_socket_fds;
	// std::string			s;
	
	fd_set				_sockets, _write_sockets, current_sockets;
	

    
	addrlen = sizeof(address);
	default_port = _get_ports(_server)[0];

	// Initializing the sockets
	FD_ZERO(&_sockets);
	FD_ZERO(&current_sockets);
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
		
		
		FD_SET(_socket_fd, &current_sockets);
		_socket_fds.push_back(_socket_fd);
		std::cerr << "fds: " << _socket_fd << std::endl;
		// _socket_fds.push_back(_socket_fd);
		// _socket_fds.push_back(_socket_fd);

	}
	int fd_size = _socket_fds[_socket_fds.size() - 1];
	int read_again = 0;
    while (1)
    {
        std::cout << "listening ..." << std::endl;
        std::cout << "fd_size: " << fd_size << std::endl;
		_sockets = current_sockets;
		if (select(fd_size + 1, &_sockets, NULL, NULL, NULL) < 0)
			print_error("error in select");

		_init_l3alam(request, response);
		// for (int i=0; i < FD_SETSIZE; i++)
		
		int x=0;
		int coming_socket;
		
		std::string _test_buffer;
		while (x<=fd_size)
		{
			std::cerr << "check return value of FD_ISSET: " << FD_ISSET(x, &_sockets) << ", at: " << x << std::endl;
			if (FD_ISSET(x, &_sockets))
			{
				// std::cerr << "hola mista: " << x << std::endl;
				if (std::find( _socket_fds.begin(), _socket_fds.end(), x) != _socket_fds.end() && !read_again)
				{
					// for (int j=0; j<_socket_fds.size(); j++)
					// 	std::cerr << "-: " << _socket_fds[j] << std::endl;
					if ((coming_socket = accept(x, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
						print_error("acception failed!");

					// request->fd = coming_socket;
					// _socket_fds.push_back(x);
					// FD_ZERO(&_sockets);
					FD_SET(coming_socket, &current_sockets);
					if (coming_socket > fd_size)
						fd_size = coming_socket;
					read_again = 1;
					std::cerr << "accepted socket: " << coming_socket << std::endl;
					break ;
					// std::cerr << "holaa: " << x << std::endl;
				}
				else if (std::find( _socket_fds.begin(), _socket_fds.end(), x) == _socket_fds.end())
				{
					// std::cerr << "empty holaa: " << coming_socket << std::endl;
					char				buffer[999999] = {0};
					int data;
					// std::cerr << "hhhh" << std::endl;
					if ((data = read(x, buffer, 999999)) < 0)
                        print_error("empty data!");
					// std::cerr << "jjjjj" << std::endl;
					// if (data < 0)
					std::cerr << "empty data: " << data << std::endl;
					if (!data || data < 999999)
					{
						// we parse req
						for (int i=0; i<data; i++)
							_test_buffer += buffer[i];
						// std::cerr << "req req: " << std::endl;
						
						// 3- Request:
						Server _s;
						_request(_server, _s, request, response, _test_buffer);

						// std::cerr << "uri: " << request->uri << std::endl;

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
						std::string s = generate_response_str(response);
						_wr = write(x, s.c_str(), s.size());
						std::cerr << "length of image: " << strlen(s.c_str()) << std::endl;
						close(x);
						FD_CLR(x, &current_sockets);
						read_again = 0;
						// _socket_fds.erase(std::remove(_socket_fds.begin(), _socket_fds.end(), x), _socket_fds.end());
						// _socket_fds.erase(std::remove(_socket_fds.begin(), _socket_fds.end(), coming_socket), _socket_fds.end());
						// for (int j=0; j<_socket_fds.size(); j++)
						// 	std::cerr << "-: " << _socket_fds[j] << std::endl;
						x++;
						// coming_socket=0;
						// std::cerr << "yooo: " << x << std::endl;
					}
					// else if (data)
					// 	break;
						
					// std::cerr << "we here??" << data << "~" << _wr << std::endl;
					// if (!data && !_wr)
					// {
					// 	// we go for parsing
					
					// }
					// else
					// {
					// 	std::cerr << "writing size: " << _wr << " ~ " << s.size() << std::endl << s << std::endl;
					// 	FD_SET(x, &_write_sockets);
					// }
				}
				else
					x++;
					// if (_wr == s.size())
					// {
					// 	
					// 	FD_CLR(x, &_write_sockets);
					// 	_wr = 0;
					// 	x++;
					// }


						
						// std::cerr << "response body: " << s << std::endl;
					

					// std::ofstream _hmida("uploads/hmida.txt");
			}
			else
				x++;
		}
	}
}
