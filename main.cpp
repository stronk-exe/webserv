/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ael-asri <ael-asri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/11 11:14:48 by ael-asri          #+#    #+#             */
/*   Updated: 2023/05/11 11:14:49 by ael-asri         ###   ########.fr       */
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

    if (ac == 2)
    {
        std::fstream file(av[1]);
        std::string _file, s;

        while (std::getline(file, s))
            (_file+=s).push_back('\n');
        
        int count = _servers_count(_file);
        t_server _server[count];

        
        // 1- Config File:
        _config_parser(_server, _file, count);

        // 2- Socket connection
        _socket(_server, _request, _response);

        
    }
    else
        std::cerr << "invalid number of arguments!" << std::endl;
    return 0;
}