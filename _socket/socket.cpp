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

std::string	generate_response_str(Client & client )
{
	std::string response = "HTTP/1.1 "+ std::to_string(client._response.status)+" "+client._response.status_message+\
					"\nContent-Type: "+client._response.content_type+\
					"\nContent-Length: "+std::to_string(client._response.content_length);
	
	if (!client.cookies.empty())
		response += "\n" + client.cookies;
	if (!client._response.location.empty())
		response += "\nLocation: "+client._response.location;
					
	response += "\n\n"+client._response.body;
	return response;
}

std::vector<int>	_get_ports( Parsing &_server )
{
    std::vector<int>    _ports;

	for (size_t i=0; i < _server.servers.size(); i++)
		_ports.push_back(_server.servers[i].listen_port);
	return _ports;
}


void	print_shit( Request &_request, Response &_response )
{
	std::cerr << "****************************************" << std::endl << "Request: " << std::endl;
	// std::cerr << "fd: " << _request.fd << std::endl;
	std::cerr << "uri: " << _request.uri << std::endl;
	std::cerr << "method: " << _request.method << std::endl;
	std::cerr << "type: " << _request.type << std::endl;
	std::cerr << "autoindex: " << _request.autoindex << std::endl;
	std::cerr << "path: " << _request.path << std::endl;
	for (size_t i=0; i<_request.index.size(); i++)
		std::cerr << "index[" << i << "]: " << _request.index[i] << std::endl;
	std::cerr << "root: " << _request.root << std::endl;
	for (size_t i=0; i<_request.redirection.size(); i++)
		std::cerr << "redirection[" << i << "]: " << _request.redirection[i] << std::endl;
	// for (size_t i=0; i<_request.cgi.size(); i++)
	// 	std::cerr << "cgi[" << i << "]: " << _request.cgi[i] << std::endl;
	// std::cerr << "client_body_upload: " << _request.client_body_upload << std::endl;
	// std::cerr << "headers: " << _request.headers << std::endl;
	std::cerr << "body: " << _request.body << std::endl;
	// for (size_t i=0; i<_request.error_pages.size(); i++)
	// 	std::cerr << "error_pages[" << i << "]: " << _request.error_pages[i] << std::endl;
	std::cerr << "is_method_allowed: " << _request.is_method_allowed << std::endl;
	std::cerr << "upload_path: " << _request.upload_path << std::endl;
	std::cerr << "upload_name: " << _request.upload_name << std::endl;
	std::cerr << "upload_content_type: " << _request.upload_content_type << std::endl;
	std::cerr << "upload_file_name: " << _request.upload_file_name << std::endl;
	std::cerr << "upload_data: " << _request.upload_data << std::endl;
	std::cerr << "boundary: " << _request.boundary << std::endl;

	std::cerr << "****************************************" << std::endl << "Response: " << std::endl;
	std::cerr << "status: " << _response.status << std::endl;
	std::cerr << "status_message: " << _response.status_message << std::endl;
	std::cerr << "content_length: " << _response.content_length << std::endl;
	std::cerr << "content_type: " << _response.content_type << std::endl;
	std::cerr << "path: " << _response.path << std::endl;
	std::cerr << "data: " << _response.data << std::endl;
	std::cerr << "body: " << _response.body << std::endl;
	std::cerr << "location: " << _response.location << std::endl;
	// std::cerr << "mims: " << _response.mims << std::endl;


}


void	_socket( Parsing &_server )
{
    int					_socket_fd;
    struct sockaddr_in	address;
    int					addrlen;
    int					default_port;
	Request		request;
	Response	response;
	std::vector<int>	_socket_fds;
	std::vector<Client> Clients;
	
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

	}
	int fd_size = _socket_fds[_socket_fds.size() - 1];
	int read_again = 0;
	int					_reading_lock=0, _writing_lock=0;
	std::string _test_buffer;
	std::string s;
	int					_wr = 0;
	int old_data=0;
	int _first_time=0;
	
    while (1)
    {
        std::cout << "listening ..." << std::endl;
		
		_sockets = _readfds;
		_current_sockets = _writefds;
		// std::cerr << "fd_size: " << fd_size << std::endl;
		if (select(fd_size + 1, &_sockets, &_current_sockets, NULL, NULL) < 0)
			print_error("error in select");
		int coming_socket;
		
		std::vector<int> accepted_shit;
		for (int x=0; x <= fd_size; x++)
		{
			if (FD_ISSET(x, &_sockets) || FD_ISSET(x, &_current_sockets))
			{
				if (std::find(_socket_fds.begin(), _socket_fds.end(), x) != _socket_fds.end() && std::find(accepted_shit.begin(), accepted_shit.end(), x) == accepted_shit.end())// && !_reading_lock && !_writing_lock)
				{
					if ((coming_socket = accept(x, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
						print_error("acception failed!");
					std::cerr << "coming_socket ::: " << coming_socket << std::endl;
					int d = fcntl(coming_socket, F_SETFL, O_NONBLOCK);
					FD_SET(coming_socket, &_readfds);
					if (coming_socket > fd_size)
						fd_size = coming_socket;
					_first_time = 1;
					_test_buffer = "";
					accepted_shit.push_back(coming_socket);
					Clients.push_back(Client(coming_socket, generateRandomString(6, "id")));
					break ;
				}
				for (size_t e=0; e<Clients.size(); e++)
				{
					// else if (std::find(_socket_fds.begin(), _socket_fds.end(), x) == _socket_fds.end() && _reading_lock)
					std::cerr << "client[" << e << "]: " << Clients[e]._id << " - " << Clients[e]._read_status << " - " << Clients[e]._write_status << std::endl;
					if ((std::find(_socket_fds.begin(), _socket_fds.end(), Clients[e]._id) == _socket_fds.end()) &&  Clients[e]._read_status)
					{
						
						char				buffer[999999] = {0};
						int data;

						if ((data = read(Clients[e]._id, buffer, 999999)) <= 0)
						{
							std::cerr << "zzzzzzzzzzzzz" << std::endl;
							Server _s;
							_request(_server, _s, Clients[e]._request, Clients[e]._response, Clients[e].buffer);
							// checking the method
							if (Clients[e]._request.is_method_allowed)
							{
								if (Clients[e]._request.method == "GET")
									_get(Clients[e], _s);
								else if (Clients[e]._request.method == "POST")
									_post(Clients[e], _s);
								else if (Clients[e]._request.method == "DELETE")
									_delete(Clients[e], _s);
							}
							else
								Clients[e]._response.status = 405;
							
							std::cerr << "status: " << Clients[e]._response.status << std::endl;
							
							
							_response(Clients[e]);
							s = generate_response_str(Clients[e]);
							Clients[e]._read_status = 0;
						}

						for (int i=0; i<data; i++)
							Clients[e].buffer += buffer[i];
						
						FD_SET(Clients[e]._id, &_writefds);
						Clients[e]._write_status = 1;
						break;

					}
					else if (std::find(_socket_fds.begin(), _socket_fds.end(), Clients[e]._id) == _socket_fds.end() &&  Clients[e]._write_status)
					{
						if (waitpid(Clients[e]._cgi_pid, &Clients[e].status, WNOHANG) > 0)
						{
							if (remove(Clients[e].file.c_str()))
               					 perror("remove file");
							///// time to exit child for 30second
							std::cerr << "Clients[e].status : " << Clients[e].status << std::endl;
							Clients[e]._kill_pid = true;
							if (WIFSIGNALED(Clients[e].status) && (WTERMSIG(Clients[e].status) == SIGALRM))
							{
								std::cerr << "@@@@@@@@@@@@@" << std::endl;

								Clients[e]._response.status = 508;
								_response(Clients[e]);
							}
							else {
								std::cerr << "---------------------Clients[e]._kill_pid : " << Clients[e]._kill_pid << std::endl;	
								parent_process( Clients[e].body, Clients[e].pipe_fd);
							std::cerr << "body : "  << Clients[e].body << std::endl;
								get_body(Clients[e]);
							std::cerr << "Clients[e]._response.content_type : "  << Clients[e]._response.content_type << std::endl;
							}
							// _get_res_body(Clients[e]);
							// std::cerr << "body : "  << Clients[e]._response.body << std::endl;
							
        					// Clients[e]._response.content_type = Clients[e]._response.mims[_get_ex(Clients[e]._request.path)];
							s = generate_response_str(Clients[e]);
						}
						if (Clients[e]._kill_pid)  
						{
							int return_write = write(Clients[e]._id, &s.c_str()[_wr], s.size()-_wr);
							_wr += return_write;
							// std::cerr << request.uri << " - Response: " << s.size() << " - Write return: " << _wr << " - reminds: " << s.size()-_wr << std::endl;
							if (return_write <= 0 || _wr == s.size())
							{
							std::cerr << "hlwaaaaaaaaaaaaaaaaaaaaaaaaa" << std::endl;
								close(Clients[e]._id);
								FD_CLR(Clients[e]._id, &_readfds);
								FD_CLR(Clients[e]._id, &_writefds);
								std::vector<Client>::iterator it = Clients.begin();
								std::advance(it, e);
								Clients.erase(it);
								_wr=0;
							}
						}
					}

				}
			}
		}
	}
}
