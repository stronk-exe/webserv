/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-jao <mait-jao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/11 11:14:48 by ael-asri          #+#    #+#             */
/*   Updated: 2023/06/22 14:23:57 by mait-jao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

std::string webserv_loc;

int main(int ac, char **av)
{
    Request *_request = new Request;
    Response *_response = new Response;
    _request->env = new char*[11];
    for (int i = 0; i < 10; i++)
        _request->env[i] = NULL;

    std::string  str;
    Parsing _server;

    
    if (ac == 3)
    {
        webserv_loc = av[2];
        _server.file = av[1];
        std::ifstream file(_server.file);
        while (std::getline(file, str))
            split_conf(_server.data, str);

        // 1- Config File:
        parss_info(_server);
        // print_data(_server);

        // 2- Socket connection
        _socket(_server, _request, _response);
    }
    else
        std::cerr << "invalid number of arguments!" << std::endl;
    return 0;
}

// void check_QueryString(std::string & path, std::string & queryString)
// {    
//     int pos = path.find('?');
//     if (pos != -1)
//         queryString = path.substr(pos, path.size() - pos); 

// 	std::cerr << "path.substr(0, pos) > " << path.substr(0, pos) << std::endl;
// 	path = path.substr(0, pos);
// }

