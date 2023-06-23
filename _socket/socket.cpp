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
					"\nLocation: "+_response->location+\
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
	// if (!_request->path.size())
	// {
		// char buffer[9999];
		// if (getcwd(buffer, sizeof(buffer)) != nullptr)
		// 	_request->path = std::string(buffer)+"/public";
		// else
		// 	_request->path = "";
	// }// char c[9999];
	// _request->path = std::string(getcwd(c, 9999));
	_request->body = "";
	_request->root = "";
	_request->method = "";
	_request->type = "";
	_request->uri = "";
	_request->autoindex = 0;
	_request->is_method_allowed = 0;
	_request->index.clear();
	_request->error_pages.clear();
	_request->cgi.clear();
	_request->redirection.clear();
	_request->headers.clear();

	// Response
	_response->status = 0;
	_response->content_length = 0;
	_response->body = "";
	_response->location = "";
	_response->status_message = "";

}

void	print_shit( Request *_request, Response *_response )
{
	std::cerr << "****************************************" << std::endl << "Request: " << std::endl;
	// std::cerr << "fd: " << _request->fd << std::endl;
	std::cerr << "uri: " << _request->uri << std::endl;
	std::cerr << "method: " << _request->method << std::endl;
	std::cerr << "type: " << _request->type << std::endl;
	std::cerr << "autoindex: " << _request->autoindex << std::endl;
	std::cerr << "path: " << _request->path << std::endl;
	for (size_t i=0; i<_request->index.size(); i++)
		std::cerr << "index[" << i << "]: " << _request->index[i] << std::endl;
	std::cerr << "root: " << _request->root << std::endl;
	for (size_t i=0; i<_request->redirection.size(); i++)
		std::cerr << "redirection[" << i << "]: " << _request->redirection[i] << std::endl;
	// for (size_t i=0; i<_request->cgi.size(); i++)
	// 	std::cerr << "cgi[" << i << "]: " << _request->cgi[i] << std::endl;
	// std::cerr << "client_body_upload: " << _request->client_body_upload << std::endl;
	// std::cerr << "headers: " << _request->headers << std::endl;
	std::cerr << "body: " << _request->body << std::endl;
	// for (size_t i=0; i<_request->error_pages.size(); i++)
	// 	std::cerr << "error_pages[" << i << "]: " << _request->error_pages[i] << std::endl;
	std::cerr << "is_method_allowed: " << _request->is_method_allowed << std::endl;
	std::cerr << "upload_path: " << _request->upload_path << std::endl;
	std::cerr << "upload_name: " << _request->upload_name << std::endl;
	std::cerr << "upload_content_type: " << _request->upload_content_type << std::endl;
	std::cerr << "upload_file_name: " << _request->upload_file_name << std::endl;
	std::cerr << "upload_data: " << _request->upload_data << std::endl;
	std::cerr << "boundary: " << _request->boundary << std::endl;

	std::cerr << "****************************************" << std::endl << "Response: " << std::endl;
	std::cerr << "status: " << _response->status << std::endl;
	std::cerr << "status_message: " << _response->status_message << std::endl;
	std::cerr << "content_length: " << _response->content_length << std::endl;
	std::cerr << "content_type: " << _response->content_type << std::endl;
	std::cerr << "path: " << _response->path << std::endl;
	std::cerr << "data: " << _response->data << std::endl;
	std::cerr << "body: " << _response->body << std::endl;
	std::cerr << "location: " << _response->location << std::endl;
	// std::cerr << "mims: " << _response->mims << std::endl;


}

void	_socket( Parsing &_server, Request *request, Response *response )
{
    int					_socket_fd;
    struct sockaddr_in	address;
    int					addrlen;
    int					default_port;
	
	std::vector<int>	_socket_fds;
	
	// std::string			s;
	
	fd_set				_sockets, _current_sockets, _readfds, _writefds;
	

    
	addrlen = sizeof(address);
	default_port = _get_ports(_server)[0];

	// Initializing the sockets
	FD_ZERO(&_sockets);
	FD_ZERO(&_current_sockets);
	FD_ZERO(&_readfds);
	FD_ZERO(&_writefds);
	for (size_t i=0; i < _server.servers.size(); i++)
	{
		// Creating a socket for each server
		if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			print_error("socket creation failed!");
		
		// Binding the sockets of each server
		memset((char *)&address, 0, sizeof(address));
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = htonl(INADDR_ANY);
		address.sin_port = htons(_server.servers[i].listen_port);

		int on = 1;
		if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)
			print_error("port in use!");
		int v = fcntl(_socket_fd, F_SETFL, O_NONBLOCK);

		if ((bind(_socket_fd, (struct sockaddr *)&address, sizeof(address))) < 0)
			print_error("binding failed!");
		
		// Start listining..
		if ((listen(_socket_fd, SOMAXCONN)) < 0)
			print_error("listining failed!");
		
		
		FD_SET(_socket_fd, &_readfds);
		_socket_fds.push_back(_socket_fd);
		std::cerr << "fds: " << _socket_fd << std::endl;
		// _socket_fds.push_back(_socket_fd);
		// _socket_fds.push_back(_socket_fd);

	}
	int fd_size = _socket_fds[_socket_fds.size() - 1];
	int read_again = 0;
	int					_reading_lock=0, _writing_lock=0;
	std::string _test_buffer;
	std::string s;
	int					_wr = 0;
	_init_l3alam(request, response);
	int old_data=0;
    while (1)
    {
        // std::cout << "listening ..." << std::endl;
		
		_sockets = _readfds;
		_current_sockets = _writefds;
		if (select(fd_size + 1, &_sockets, &_current_sockets, NULL, NULL) < 0)
			print_error("error in select");
		
		int x=0;
		int coming_socket;
		
		
		
		
		
		while (x <= fd_size)
		{
			// std::cerr << "check return value of FD_ISSET: " << FD_ISSET(x, &_sockets) << ", at: " << x << std::endl;
			// std::cerr << "check return value of FD_ISSET current: " << FD_ISSET(x, &_current_sockets) << ", at: " << x << std::endl;
			if (FD_ISSET(x, &_sockets) || FD_ISSET(x, &_current_sockets))
			{
				// std::cerr << "hola mista: " << x << "-" << _reading_lock << "-" << _writing_lock << std::endl;
				// for (size_t f=0; f<_socket_fds.size(); f++)
				// 	std::cerr << "_socket_fds[" << f << "]: " << _socket_fds[f] << std::endl;
				if (std::find(_socket_fds.begin(), _socket_fds.end(), x) != _socket_fds.end() && !_reading_lock && !_writing_lock)
				{
					std::cerr << "sssssssssssssssssssssss" << std::endl;
					if ((coming_socket = accept(x, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
						print_error("acception failed!");
					int d = fcntl(coming_socket, F_SETFL, O_NONBLOCK);
					FD_SET(coming_socket, &_readfds);
					if (coming_socket > fd_size)
						fd_size = coming_socket;
					_reading_lock = 1;
					_test_buffer = "";

					break ;
				}
				else if (std::find(_socket_fds.begin(), _socket_fds.end(), x) == _socket_fds.end() && _reading_lock)
				{
					char				buffer[999999] = {0};
					int data;

					if ((data = read(x, buffer, 999999)) <= 0)
					{
						// for (int i=0; i<(old_data-data); i++)
						// 	_test_buffer += buffer[i];
						FD_SET(x, &_writefds);
						
						Server _s;
						_request(_server, _s, request, response, _test_buffer);
						std::cerr << "shity read: " << data << " - " << old_data << std::endl;

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
						
						_response(response, request);
						s = generate_response_str(response);
						_reading_lock = 0;
						_writing_lock = 1;
						// std::cerr << "PATH: " << request->path << std::endl;
						x++;
						// FD_CLR(x, &_readfds);
						// std::cerr << "buffer: " << _test_buffer << std::endl;
						break;
					}
					old_data = data;
					std::cerr << "rddddddddddddddddddddd" << data << std::endl;
					for (int i=0; i<data; i++)
						_test_buffer += buffer[i];
					// request->fd = x;
					// FD_SET(x, &_readfds);
					// x++;
				}
				else if (std::find(_socket_fds.begin(), _socket_fds.end(), x) == _socket_fds.end() && _writing_lock)
				{
					std::cerr << "wrrrrrrrrrrrrrrrrrrrrr" << std::endl;
					int return_write = write(x, &s.c_str()[_wr], s.size()-_wr);
					_wr += return_write;
					// std::cerr << request->uri << " - Response: " << s.size() << " - Write return: " << _wr << " - reminds: " << s.size()-_wr << std::endl;
					if (return_write <= 0 || _wr == s.size())
					{
						close(x);
						FD_CLR(x, &_readfds);
						FD_CLR(x, &_writefds);
						_writing_lock = 0;
						_wr=0;
						std::cerr << "l3zz: " << response->content_type << " - " << response->content_length << std::endl;
						_test_buffer = "";
						_init_l3alam(request, response);
						// print_shit(request, response);
						x++;
					}
					x++;

				}
				else
					x++;
			}
			else
				x++;
		}
	}
}
