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

std::string num_to_str(ssize_t num)
{
	std::ostringstream str1;
    str1 << num;
    return str1.str();
}

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

// void	_init_l3alam( Request &_request, Response &_response)
// {
// 	// Request
// 	// if (!_request.path.size())
// 	// {
// 		// char buffer[9999];
// 		// if (getcwd(buffer, sizeof(buffer)) != nullptr)
// 		// 	_request.path = std::string(buffer)+"/public";
// 		// else
// 		// 	_request.path = "";
// 	// }// char c[9999];
// 	// _request.path = std::string(getcwd(c, 9999));
// 	_request.body = "";
// 	_request.root = "";
// 	_request.method = "";
// 	_request.type = "";
// 	_request.uri = "";
// 	_request.autoindex = 0;
// 	_request.is_method_allowed = 0;
// 	_request.index.clear();
// 	_request.error_pages.clear();
// 	_request.cgi.clear();
// 	_request.redirection.clear();
// 	_request.headers.clear();

// 	// Response
// 	_response.status = 0;
// 	_response.content_length = 0;
// 	_response.body = "";
// 	_response.location = "";
// 	_response.status_message = "";

// }

// void	print_shit( Request &_request, Response &_response )
// {
// 	//std::cerr << "****************************************" << std::endl << "Request: " << std::endl;
// 	// //std::cerr << "fd: " << _request.fd << std::endl;
// 	//std::cerr << "uri: " << _request.uri << std::endl;
// 	//std::cerr << "method: " << _request.method << std::endl;
// 	//std::cerr << "type: " << _request.type << std::endl;
// 	//std::cerr << "autoindex: " << _request.autoindex << std::endl;
// 	//std::cerr << "path: " << _request.path << std::endl;
// 	for (size_t i=0; i<_request.index.size(); i++)
// 		//std::cerr << "index[" << i << "]: " << _request.index[i] << std::endl;
// 	//std::cerr << "root: " << _request.root << std::endl;
// 	for (size_t i=0; i<_request.redirection.size(); i++)
// 		//std::cerr << "redirection[" << i << "]: " << _request.redirection[i] << std::endl;
// 	// for (size_t i=0; i<_request.cgi.size(); i++)
// 	// 	//std::cerr << "cgi[" << i << "]: " << _request.cgi[i] << std::endl;
// 	// //std::cerr << "client_body_upload: " << _request.client_body_upload << std::endl;
// 	// //std::cerr << "headers: " << _request.headers << std::endl;
// 	//std::cerr << "body: " << _request.body << std::endl;
// 	// for (size_t i=0; i<_request.error_pages.size(); i++)
// 	// 	//std::cerr << "error_pages[" << i << "]: " << _request.error_pages[i] << std::endl;
// 	//std::cerr << "is_method_allowed: " << _request.is_method_allowed << std::endl;
// 	//std::cerr << "upload_path: " << _request.upload_path << std::endl;
// 	//std::cerr << "upload_name: " << _request.upload_name << std::endl;
// 	//std::cerr << "upload_content_type: " << _request.upload_content_type << std::endl;
// 	//std::cerr << "upload_file_name: " << _request.upload_file_name << std::endl;
// 	//std::cerr << "upload_data: " << _request.upload_data << std::endl;
// 	//std::cerr << "boundary: " << _request.boundary << std::endl;

// 	//std::cerr << "****************************************" << std::endl << "Response: " << std::endl;
// 	//std::cerr << "status: " << _response.status << std::endl;
// 	//std::cerr << "status_message: " << _response.status_message << std::endl;
// 	//std::cerr << "content_length: " << _response.content_length << std::endl;
// 	//std::cerr << "content_type: " << _response.content_type << std::endl;
// 	//std::cerr << "path: " << _response.path << std::endl;
// 	//std::cerr << "data: " << _response.data << std::endl;
// 	//std::cerr << "body: " << _response.body << std::endl;
// 	//std::cerr << "location: " << _response.location << std::endl;
// 	// //std::cerr << "mims: " << _response.mims << std::endl;


// }

// std::string getSubstring(const std::string str, std::string::size_type startPos, std::string::size_type length) {
// 	//std::cerr << "s.size : " << str.size()  << std::endl;

//     if (startPos > str.length())
//         return "";

//     return str.substr(startPos, length);
// }

int find_client(std::vector <Client> _cls, int _id)
{
	for (int i =0; i < _cls.size(); i++)
	{
		if (_cls[i]._id == _id)
			return _id;
	}
	return -1;
}


void	_socket( Parsing &_server )
{
    int					_socket_fd;
    struct sockaddr_in	address;
    int					addrlen;
    int					default_port;
	std::vector<int>	_socket_fds;
	std::vector<Client> Clients;
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

	}
	int fd_size = _socket_fds[_socket_fds.size() - 1];
	int read_again = 0;
	int					_reading_lock=0, _writing_lock=0;
	std::string _test_buffer;
	// int old_data=0;
	// int _first_time=0;
	// int a_key=0;
	std::vector<int> accepted_shit;
    while (1)
    {
        std::cout << "listening ..." << std::endl;
		
		_sockets = _readfds;
		_current_sockets = _writefds;
		// //std::cerr << "fd_size: " << fd_size << std::endl;
		if (select(fd_size + 1, &_sockets, &_current_sockets, NULL, NULL) < 0)
			print_error("error in select");
		
		// int x=0;
		int coming_socket;
		
		
		
		// _first_time=0;
		// while (x <= fd_size)
		//std::cerr << "Clients.size : " << Clients.size() <<std::endl;
		for (int x=0; x <= fd_size; x++)
		{


			if (FD_ISSET(x, &_sockets) || FD_ISSET(x, &_current_sockets))
			{
				for (size_t e=0; e<Clients.size(); e++)
				{
					if (FD_ISSET(Clients[e]._id, &_sockets) && FD_ISSET(Clients[e]._id, &_current_sockets)) {
						//std::cerr << "\e[96m###############\e[0m : fd_file : " << Clients[e]._id << std::endl;
						close(Clients[e]._id);
						FD_CLR(Clients[e]._id, &_readfds);
						FD_CLR(Clients[e]._id, &_writefds);
						std::vector<Client>::iterator it = Clients.begin();
						std::advance(it, e);
						Clients.erase(it);
					}
				}


				if (std::find(_socket_fds.begin(), _socket_fds.end(), x) != _socket_fds.end() /*&& std::find(accepted_shit.begin(), accepted_shit.end(), x+1) != accepted_shit.end()*/)
				{
					//std::cerr << "\e[31mACCEPTINGGGGGGGGGGGGGG\e[0m" << x << std::endl;
					if ((coming_socket = accept(x, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
						print_error("acception failed!");
						int d = fcntl(coming_socket, F_SETFL, O_NONBLOCK);
						FD_SET(coming_socket, &_readfds);
						if (coming_socket > fd_size)
							fd_size = coming_socket;
						Clients.push_back(Client(coming_socket));
					break ;
				}
				
				for (size_t e=0; e<Clients.size(); e++)
				{
					if (x == Clients[e]._id && (std::find(_socket_fds.begin(), _socket_fds.end(), Clients[e]._id) == _socket_fds.end()) &&  Clients[e]._read_status)
					{
						//std::cerr << "\e[32mREADINGGGGGGGGGGGGGG \e[0m _id : "<< Clients[e]._id << std::endl;
						
						char				buffer[999999] = {0};

						Clients[e].data = read(Clients[e]._id, buffer, 999999);
						if (Clients[e].data > 0)
						{
							for (int i=0; i<Clients[e].data; i++)
								Clients[e].buffer += buffer[i];
							Clients[e].prsing_req = Clients[e].buffer;
						}

						if (Clients[e].data <= 0 || Clients[e].data < 999999)
						{
								
								Clients[e]._done_reading = 1;
								Clients[e]._read_status = 0;
								FD_SET(Clients[e]._id, &_writefds);
						}
						if (errno != 0) {
								std::cerr << " HOLA :" << errno << std::endl; // Print a descriptive error message
								// Handle the error
							}
					}
					// Request parsing
					else if (x == Clients[e]._id && Clients[e]._done_reading && !Clients[e]._read_status && !Clients[e]._write_status)
					{
						// Clients[e].data = 0;
						//std::cerr << "\e[35m---- \e[0m_id : "<< Clients[e]._id  << " - size_doo : " << Clients[e].doo.size() << std::endl;
						//std::cerr << "\e[33mPARSINGGGGGGGGGGGGGG \e[0m_id : "<< Clients[e]._id  << " - size_buffer : " << Clients[e].buffer.size() << std::endl;
						Server _s;
						_request(_server, _s, Clients[e]._request, Clients[e]._response, Clients[e].prsing_req);
	
						// Checking the method
						
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
						{
							//std::cerr << "\e[38;5;78m********************************\e[0m" << std::endl;
							Clients[e]._response.status = 405;
						}
						
						_response(Clients[e]);
						
						Clients[e].s = generate_response_str(Clients[e]);
						Clients[e]._write_status = 1;
						FD_SET(Clients[e]._id, &_writefds);
					}
					else if (x == Clients[e]._id && std::find(_socket_fds.begin(), _socket_fds.end(), Clients[e]._id) == _socket_fds.end() && Clients[e]._write_status/* && !Clients[e]._done_writing*/)
					{
						//std::cerr << "\e[34mWRITINGGGGGGGGGGGGGG \e[0m_id : "<< Clients[e]._id << std::endl;
						if (waitpid(Clients[e]._cgi_pid, &Clients[e].status, WNOHANG) > 0)
						{
							if (remove(Clients[e].file.c_str()))
               					 perror("remove file");
							///// time to exit child for 30second
							// //std::cerr << "Clients[e].status : " << Clients[e].status << std::endl;
							Clients[e]._kill_pid = true;
							if (WIFSIGNALED(Clients[e].status) && (WTERMSIG(Clients[e].status) == SIGALRM))
							{
								//std::cerr << "@@@@@@@@@@@@@" << std::endl;

								Clients[e]._response.status = 508;
								_response(Clients[e]);
							}
							else {
								//std::cerr << "---------------------Clients[e]._kill_pid : " << Clients[e]._kill_pid << std::endl;	
								parent_process( Clients[e].body, Clients[e].pipe_fd);
							//std::cerr << "body : "  << Clients[e].body << std::endl;
								get_body(Clients[e]);
							//std::cerr << "Clients[e]._response.content_type : "  << Clients[e]._response.content_type << std::endl;
							}

							Clients[e].s = generate_response_str(Clients[e]);
						}
						if (Clients[e]._kill_pid)
						{
							Clients[e].return_write = write(Clients[e]._id, &Clients[e].s[Clients[e]._wr], Clients[e].s.size()-Clients[e]._wr);
							if (Clients[e].return_write > 0)
								Clients[e]._wr += Clients[e].return_write;
							if (Clients[e]._done_writing /*Clients[e]._wr == Clients[e]._response.content_length*/)
							{
								//std::cerr << "\e[96m###############\e[0m : fd_file : " << Clients[e]._id << std::endl;
								close(Clients[e]._id);
								FD_CLR(Clients[e]._id, &_readfds);
								FD_CLR(Clients[e]._id, &_writefds);
								std::vector<Client>::iterator it = Clients.begin();
								std::advance(it, e);
								Clients.erase(it);
								break;
							}
							else// if (Clients[e].return_write == Clients[e].s.size()-Clients[e]._wr)
							{
								_get_res_body(Clients[e]);
								Clients[e].s = generate_response_str(Clients[e]);
								//std::cerr << "lblan dyal s: " << Clients[e].s.size() << std::endl;
							}
						// x++;
						}

					}
					// else
					// 	x++;

				}
				// x++;
				
			}
			// else
			// 	x++;
		}
	}
}
