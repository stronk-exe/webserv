/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-jao <mait-jao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/11 11:15:45 by ael-asri          #+#    #+#             */
/*   Updated: 2023/06/11 20:39:38 by mait-jao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp"

std::string	_get_ex( std::string _file_name )
{
	size_t dotPos = _file_name.find_last_of('.');
    if (dotPos != std::string::npos && dotPos < _file_name.length() - 1) {
        return _file_name.substr(dotPos + 1);
    }
	return "";
}

size_t getFileSize(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        //std::cerr << "Failed to open file: " << filename << std::endl;
        return -1;
    }

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);

    fclose(file);

    return fileSize;
}

int _get_res_body( Client & _client )
{
    // //std::cerr << "\e[93myo fd file for -:\e[0m" << _client._id << "\e[92m- is \e[0m" << _client.fd_file << std::endl;

    if (!_client.fd_file)
    {
        _client.fd_file = open( _client._request.path.c_str(), O_RDONLY );
        //std::cerr << "file length: " << getFileSize(_client._request.path.c_str()) << std::endl;
        _client._response.content_length = getFileSize(_client._request.path.c_str());
    }
    int data=1;
    // while (data>0)
    // {
        char buffer[999999] = {0};
        data = read(_client.fd_file, buffer, 999999);
        for (int i=0; i<data; i++)
            _client._response.body += buffer[i];
        // //std::cerr << "file data: " << _client.fd_file << " ---- " << data << std::endl;
        if (data>0)
            _client._done_writing = 0;
        else
        {
            // //std::cerr << "PPPPPPPPPP" << std::endl;
            _client._done_writing = 1;
            close(_client.fd_file);
        }
        if (_client._response.content_type.empty())
        {
            // //std::cerr << "................" << std::endl;
            _client._response.content_type = _client._response.mims[_get_ex(_client._request.path)];
            if (!_client._response.content_type.size())
                _client._response.content_type = "text/html";
            // else
        }
        if (!_client._response.content_length)
            _client._response.content_length = _client._response.body.size();
        // //std::cerr <<"size of res_body: " << _client._response.body.size() << std::endl;
    // }
    
    // //std::cerr << "w3lach: " << _client._response.content_length << std::endl;
    
    
    
    // //std::cerr << "yyooo: " << _request.path << " ~ " << _get_ex(_request.path) << " ~ " << _response.content_type << std::endl;
	// std::ifstream myfile;
	// myfile.open(_request.path);
    // //std::cerr << "path: " << _request.path << std::endl;
	// std::string myline;
	// _response.body = "";
	// if ( myfile.is_open() )
	// {
	// 	while ( myfile ) {
	// 		std::getline (myfile, myline);
	// 		_response.body += myline;
	// 	}
	// }
	// else
	// 	std::cout << "Couldn't open file\n";
	// _response.body.erase(_response.body.size()-myline.size(), myline.size());
	return 1;
}

void    get_indexed_file_data( Request &_request, Response &_response, std::string path )
{
    for (size_t i=0; i<_request.index.size(); i++)
    {
        std::ifstream myfile;
        myfile.open(path+_request.index[i]);
        std::string myline;
        _response.body = "";
        if ( myfile.is_open() )
        {
            while ( myfile ) {
                std::getline (myfile, myline);
                _response.body += myline;
            }
        }
        myfile.close();
    }
}

void    get_file_data( Response &_response, std::string path )
{
    std::ifstream myfile;
	myfile.open(path);
	std::string myline;
	_response.body = "";
	if ( myfile.is_open() )
	{
		while ( myfile ) {
			std::getline (myfile, myline);
			_response.body += myline;
		}
	}
	else
		std::cout << "Couldn't open file\n";
    myfile.close();
}

void	_response( Client & _client )
{
	int _status_found=0;
    if (_client._request.error_pages.size())
    {
        for (size_t i=0; i<_client._request.error_pages.size(); i++)
        {
            for (size_t j=0; j<_client._request.error_pages[i].error_status[j]; j++)
            {
				if (_client._response.status == _client._request.error_pages[i].error_status[j])
                {
                    // //std::cerr << "ayooo: " << _client._request.error_pages[i].path << " - " << _client._request.error_pages.size() << std::endl;
					get_file_data(_client._response, _client._request.error_pages[i].path);
                    _status_found=1;
                }
            }
        }
    }
    
	// //std::cerr << "-----------------------------" << std::endl;

    if (!_status_found && _client._response.status != 200)
    {
        //std::cerr << "status found: " << _status_found << " status: " << _client._response.status << std::endl;
        if (_client._response.status == 204)
        {
            _client._response.body = "<html><body><h1>204 No Content</h1><img src=\"https://cdn.hashnode.com/res/hashnode/image/upload/v1611008552253/F5teDDfzj.png?auto=compress,format&format=webp\" alt=\"bad request\"/></body></html>";
            _client._response.status_message = "No Content";
        }
        else if (_client._response.status == 400)
        {
            _client._response.body = "<html><body><h1>400 Bad Request</h1><img src=\"https://cdn.hashnode.com/res/hashnode/image/upload/v1611008552253/F5teDDfzj.png?auto=compress,format&format=webp\" alt=\"bad request\"/></body></html>";
            _client._response.status_message = "Bad Request";
        }
        else if (_client._response.status == 403)
        {
            _client._response.body = "<html><body><h1>403 Forbidden</h1><img src=\"https://i.kym-cdn.com/entries/icons/original/000/028/434/We_Believe__The_Best_Men_Can_Be___Gillette_(Short_Film)_1-4_screenshot.jpg\" alt=\"forbidden\"/></body></html>";
            _client._response.status_message = "Forbidden";
        }
        else if (_client._response.status == 404)
        {
            _client._response.body = "<html><body><h1>404 Not Found</h1><img src=\"https://media.makeameme.org/created/file-not-found-c17b083c9c.jpg\" alt=\"404_not_found.gif\"/></body></html>";
            _client._response.status_message = "Not Found";
        }
        else if (_client._response.status == 405)
        {
            _client._response.body = "<html><body><h1>405 Method Not Allowed</h1><img src=\"https://en.meming.world/images/en/a/a3/We_Don%27t_Do_That_Here.jpg\" alt=\"method_not_allowed\"/></body></html>";
            _client._response.status_message = "Method Not Allowed";
        }
        else if (_client._response.status == 409)
        {
            _client._response.body = "<html><body><h1>409 Conflict</h1><img src=\"https://en.meming.world/images/en/a/a3/We_Don%27t_Do_That_Here.jpg\" alt=\"method_not_allowed\"/></body></html>";
            _client._response.status_message = "Conflict";
        }
        else if (_client._response.status == 413)
        {
            _client._response.body = "<html><body><h1>413 Request Entity Too Large</h1><img src=\"https://preview.redd.it/request-entity-too-large-all-of-a-sudden-pics-i-could-send-v0-dqgu5n5guhh91.jpg?auto=webp&s=a83ff042398f7dc5cbb36cf21ae8b1fc97bc7b68\" alt=\"request_entity_too_large\"/></body></html>";
            _client._response.status_message = "Request Entity Too Large";
        }
        else if (_client._response.status == 414)
        {
            _client._response.body = "<html><body><h1>414 Request-URI Too Long</h1><img src=\"https://www.catalystdigital.com/wp-content/uploads/url-too-long.jpg\" alt=\"request_uri_too_long\"/></body></html>";
            _client._response.status_message = "Request-URI Too Long";
        }
        else if (_client._response.status == 500)
        {
            _client._response.body = "<html><body><h1>500 Internal Server Error</h1><img src=\"https://3.bp.blogspot.com/-l_OPWrz4AZo/VtLroz9u1cI/AAAAAAAANPU/mGoZb0ZKwdk/s1600/zytel.jpg\" slt=\"not_implemented\"/></body></html>";
            _client._response.status_message = "Internal Server Error";
        }
        else if (_client._response.status == 501)
        {
            _client._response.body = "<html><body><h1>501 Not Implemented</h1><img src=\"https://3.bp.blogspot.com/-l_OPWrz4AZo/VtLroz9u1cI/AAAAAAAANPU/mGoZb0ZKwdk/s1600/zytel.jpg\" slt=\"not_implemented\"/></body></html>";
            _client._response.status_message = "Not Implemented";
        }
        else if (_client._response.status == 508)
        {
            _client._response.body = "<html><body><h1>508 Loop Detected</h1><img src=\"https://d3mvlb3hz2g78.cloudfront.net/wp-content/uploads/2017/12/thumb_720_450_Haiku_Stairsdreamstime_xl_50641068_1.jpg\" slt=\"Loop Detected\"/></body></html>";
            _client._response.status_message = "Loop Detected";
        }
        _client._response.content_type = "text/html";
    }
    // else
    // {
	// }

	// Response heders
	// if (_response.content_length <= 0)
    //     _response.content_length = (_response.body).size();
    // // //std::cerr << "Content Lenght: " << _response.content_length << " | body: " << _response.body << std::endl;
	// if (!_response.content_type.size())
    //     _response.content_type = "text/html";
    // //std::cerr << "wa lwzz: " << _response.content_type << "~" << _response.status << std::endl;
	if (_client._response.body.empty() && _client._kill_pid)
	{
    	_get_res_body(_client);
        // _client._response.content_type = _client._response.mims[_get_ex(_client._request.path)];
    }
    if (!_client._response.content_length)
        _client._response.content_length = _client._response.body.size();
    
    // //std::cerr << "gg: " << _client._response.content_type << std::endl;
}