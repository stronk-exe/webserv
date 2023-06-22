/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-jao <mait-jao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/16 13:05:56 by mait-jao          #+#    #+#             */
/*   Updated: 2023/06/22 14:34:44 by mait-jao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp" 


std::string int_to_str(int num)
{
    std::ostringstream str1;
    str1 << num;
    return str1.str();
}

void update_env_for_cgi( Request *_request , std::string _path_info, Server _server )
{///////////
    std::cerr << "{\033[38;5;108m" << _request->path<< "\033[0m}"<< std::endl;
    for (int i = 0; i < 10; i++)
    {
        if (_request->env[i])
            delete _request->env[i];
    }
    ////////);
    std::cerr <<  "\033[38;5;208mqueryString : \033[0m"  << _request->queryString << std::endl;
    _request->env[0] = strdup(("PATH_INFO=" + _path_info).c_str());
    _request->env[1] = strdup(("REQUEST_METHOD=" + _request->method).c_str());
    _request->env[2] = strdup(("CONTENT_TYPE=" + _request->headers["Content-Type"]).c_str());
    _request->env[3] = strdup(("CONTENT_LENGTH=" + _request->headers["Content-Length"]).c_str());
    _request->env[4] = strdup(("SCRIPT_FILENAME=" + _path_info).c_str());
    _request->env[5] = strdup(("SERVER_PORT=" + int_to_str(_server.listen_port)).c_str());
    _request->env[6] = strdup(("SERVER_NAME=" + _server.name).c_str());
    _request->env[7] = strdup(("HTTP_COOKIE=" + _request->headers["Cookie"]).c_str());
    _request->env[8] = strdup(("QUERY_STRING=" + _request->queryString).c_str());
    _request->env[9] = strdup("REDIRECT_STATUS=200");
    _request->env[10] = NULL;
}

void     printHeaders(std::map<std::string, std::string> headers)
{
    std::map<std::string, std::string>::iterator i;
        // std::cerr << i->first << " " << i->second << std::endl;
        std::cerr << "*********************************************" << std::endl;
    for ( i = headers.begin(); i != headers.end(); i++)
    {
        std::cerr << i->first << " -> " << i->second << std::endl;
    }
        std::cerr << "*********************************************" << std::endl;
}

bool check_extension(std::string extension, std::string path)
{
    std::string::reverse_iterator it_ext, it_path;
    
    extension.insert(0, ".");
    for (it_ext = extension.rbegin(), it_path = path.rbegin(); it_ext != extension.rend() ; it_ext++, it_path++)
    {
        if (*it_ext != *it_path)
            return false;
    }
    return true;
}

bool check_path_extension(std::vector<CGI>	&cgi_pass, std::string &path, std::string &scriptPath )
{
    std::vector<CGI>::iterator it;
 
    for (it = cgi_pass.begin(); it != cgi_pass.end() ; it++)
    {
        if (check_extension((*it).extension, path))
        {
            scriptPath = (*it).path + " " + path;
            return true;
        }
    }
    return false;
}

std::string get_content_type(char *buffer)
{
    std::string str;
    
    char* token = std::strtok(buffer, " ");
    token = std::strtok(NULL, " ");
    
    return (str = token);
}


void parent_process(std::string &result, int *pipe_fd, int pid)
{
    char buffer[4056];
    int status, nbytes;
    close(pipe_fd[1]);
    while ((nbytes = read(pipe_fd[0], buffer, sizeof(buffer))) > 0)
    {
        // std::cerr << "..................................." << std::endl;
        result.append(buffer, nbytes);
    }
    close(pipe_fd[0]);
    wait(NULL);
}


int createFile(const char* filename, std::string data) {
    
    int fd = open(filename, O_WRONLY | O_CREAT, 0644);
    if (fd != -1) {
        write(fd, data.c_str(), data.size());
        // close(fd); 
    } else {
        std::cerr << strerror(errno) << std::endl;
        exit(12);
    }
    close(fd);
    fd = open(filename, O_RDONLY, 0644);
    return fd;
}

std::string exec_file_cgi(std::string &scriptPath, std::string &_pwd, Request *_request)
{
    pid_t pid;
    char *av[3];
    int fd, pipe_fd[2];
    (void)_pwd;
    std::string result, arg, args[2];
    
    std::istringstream ss(scriptPath);
    for (int i = 0; i < 2; i++) {
        std::getline(ss, arg, ' ');
        args[i] = arg;
        av[i] = const_cast<char*>(args[i].c_str());
        // std::cerr << av[i] << std::endl;
    }
    av[2] = NULL;
    
        // std::cerr << "=======================================" << std::endl;
        // std::cerr << "body : \n" << _request->body << std::endl;
        // std::cerr << "=======================================" << std::endl;
    fd = createFile(".hamid", _request->body);
    if (!(pipe(pipe_fd) > -1 && (pid = fork()) > -1)) {
        std::cerr << strerror(errno) << std::endl;
        exit(12);
    }
    if (pid == 0) {
        dup2(fd, STDIN_FILENO);
        // close(fd);
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
        execve(av[0], av, _request->env);
        std::cerr << strerror(errno) << std::endl;
        exit(12);
    } else {
        close(fd);
        // if (remove(".hamid"))
        //     perror("remove file");
        parent_process(result, pipe_fd, pid);///////////////////
    }
    if (remove(".hamid"))
        perror("remove file");
    // write (2, "*********", 10);
    return result;
}

void get_body(Response * _response, std::string & result)
{
    std::string arg;
    std::istringstream ss(result);
       
    int pos0, pos1;

    pos0 = result.find("Content-Type:");
    if (pos0 == -1)
        pos0 = result.find("Content-type:");
    pos1 = result.find("\n", pos0);
    if (pos0 != -1)
    {
        _response->content_type = result.substr(pos0 + 13 , pos1 - pos0 - 13);
        result.erase(0, pos1 + 1);
    }
    else
        _response->content_type = "text/html";
        
    _response->body = result;
    _response->content_length = _response->body.size();
    _response->status = 200;
}

void	_cgi( Request *_request, Response *_response , Server &_server )
{
    // printHeaders(_request->headers); 
	std::string result, scriptPath, arg, _pwd;
    // std::cerr << "-------------------------------" << std::endl;
    std::cerr << "\033[38;5;200mCGI\033[0m" << std::endl;
    // std::cerr << "path in cgi: " << _request->path << std::endl;
    _pwd = getcwd(NULL, 0);
    update_env_for_cgi(_request, (_request->path), _server);
    if (!check_path_extension(_request->cgi , _request->path, scriptPath)) {
        std::cerr << "extansion" << std::endl;
        _response->body = "";
        return ;
    }
    // for (int i = 0; i < 10; i++)
        // std::cerr << "|" << _request->env[i] << "|" << std::endl;

    // std::cerr << "jjjjjj: " << scriptPath << std::endl;
    result = exec_file_cgi(scriptPath, _pwd, _request);
	get_body(_response, result);
    
	// std::cerr << "execution output: |" << _response->body <<"|" << std::endl;
	// std::cerr << "*************************************************" << std::endl;  
}
