/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-jao <mait-jao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/11 11:14:48 by ael-asri          #+#    #+#             */
/*   Updated: 2023/05/16 15:31:27 by mait-jao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int _servers_count( std::string s )
{
    std::string str= "server {";
    size_t nPos = s.find(str, 0); 
    int count = 0;
    while (nPos != std::string::npos)
    {
        count++;
        nPos = s.find(str, nPos + str.size());
    }
    return count;
}

int main(int ac, char **av)
{
    // Requestuest *_request;
    // Responseponse *_response;
    Request *_request = new Request;
    Response *_response = new Response;

    
    std::string  str;
    Parsing _server;

    if (ac == 2)
    {
        // std::fstream file(av[1]);
        // std::string _file, s;

        // while (std::getline(file, s))
        //     (_file+=s).push_back('\n');
        
        // int count = _servers_count(_file);
        // t_server _server[count];
        _server.file = av[1];
        std::ifstream file(_server.file);
        while (std::getline(file, str))
            split_conf(_server.data, str);

        
        // 1- Config File:
        // _config_parser(_server, _file, count);
        (void)_request;
        (void)_response;
        parss_info(_server);
        std::cout << (_server.servers.begin())->name << std::endl;
        std::cout << (_server.servers.begin())->listen_port << std::endl;
        // print_data(_server);
        // exit(1);

        // 2- Socket connection
        // _socket(_server, _request, _response);

        
    }
    else
        std::cerr << "invalid number of arguments!" << std::endl;
    return 0;
}