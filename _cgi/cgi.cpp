/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-jao <mait-jao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/16 13:05:56 by mait-jao          #+#    #+#             */
/*   Updated: 2023/06/07 20:32:24 by mait-jao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../webserv.hpp" 

// bool get_first_arg(char str[], std::string &s)
// {
//     char *token = strtok(str, " ");
//     if (token[0] == '#' && token[1] == '!') {
//         s = token;
//         return true;
//     }
//     return false;
// }

// void get_command(std::string &str, std::string _file)
// {
//     std::ifstream file(_file);
//     while (std::getline(file, str))
//     {
//         if (get_first_arg(const_cast<char *>(str.c_str()), str))
//             break;
//     }
//     str.erase(str.begin(), str.begin() + 2);
// }

// void path_file(std::string root, std::string file, char **str)
// {
//     std::string tmp = root + file;/////dosnt work 
//     *str = const_cast<char *>(tmp.c_str());
// }

std::string int_to_str(int num)
{
    std::ostringstream str1;
    str1 << num;
    return str1.str();
}

void update_env_for_cgi( Request *_request , std::string _path_info, Server _server )
{
    if (setenv("PATH_INFO", _path_info.c_str(), 1) != 0) {
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }
    if (setenv("REQUEST_METHOD", _request->method.c_str(), 1) != 0) {
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }
    if (setenv("CONTENT_TYPE", _request->headers["CONTENT_TYPE"].c_str(), 1) != 0) {
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }
    if (setenv("CONTENT_LENGTH", _request->headers["CONTENT_LENGTH"].c_str(), 1) != 0) {
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }
    if (setenv("HTTP_USER_AGENT", _request->headers["User-Agent"].c_str(), 1) != 0) {
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }
    if (setenv("DOCUMENT_ROOT", _request->root.c_str(), 1) != 0) {
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }
    if (setenv("SCRIPT_NAME", _request->path.c_str(), 1) != 0) {
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }
    if (setenv("REMOTE_ADDR", "127.0.0.1", 1) != 0) {/////hardcod
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }    
    if (setenv("REMOTE_HOST", _request->headers["Host"].c_str(), 1) != 0) {/////hardcod
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }
    if (setenv("SERVER_PORT", int_to_str(_server.listen_port).c_str(), 1) != 0) {/////hardcod
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }    
    if (setenv("SERVER_NAME", _server.name.c_str(), 1) != 0) {/////hardcod
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }    
    if (setenv("HTTP_COOKIE", _request->headers["Cookie"].c_str(), 1) != 0) {/////hardcod
        std::cerr << "Failed to set environment variable." << std::endl;
        return ;
    }
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


bool parent_process(std::string &result, int *pipe_fd, int pid)
{
    char buffer[256];
    int status, nbytes;
    close(pipe_fd[1]);
    while ((nbytes = read(pipe_fd[0], buffer, sizeof(buffer))) > 0)
        result.append(buffer, nbytes);
    close(pipe_fd[0]);
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        return true;    
    return false;
}

bool exec_cgi(std::string &scriptPath, std::string &result, std::string &_pwd)
{
    pid_t pid;
    char *av[3];
    int pipe_fd[2];
    std::string arg, args[2];
    

    std::istringstream ss(scriptPath);
    for (int i = 0; i < 2; i++) {
        std::getline(ss, arg, ' ');
        args[i] = _pwd + "/" + arg;
        av[i] = const_cast<char*>(args[i].c_str());
    }
    av[2] = NULL;
    if (!(pipe(pipe_fd) > -1 && (pid = fork()) > -1)) {
        std::cerr << strerror(errno) << std::endl;
        exit(12);
    }
    if (pid == 0) {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
        execve(av[0], av, NULL);
        std::cerr << av[0] << " | " << av[1] << " | " << av[2] << std::endl;
        std::cerr << strerror(errno) << std::endl;
        exit(12);
    } else {
        parent_process(result, pipe_fd, pid);
    }
    return false;
}

int createFile(const char* filename, std::string data) {
    
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd != -1) {
        write(fd, data.c_str(), data.size());
        close(fd); 
    } else {
        std::cerr << strerror(errno) << std::endl;
        exit(12);
    }
    return fd;
}

bool exec_file_cgi(std::string &scriptPath, std::string &result, std::string &_pwd, std::string &data)
{
    pid_t pid;
    char *av[3];
    int fd_file, pipe_fd[2];
    std::string arg, args[2];
    

    std::istringstream ss(scriptPath);
    for (int i = 0; i < 2; i++) {
        std::getline(ss, arg, ' ');
        args[i] = _pwd + "/" + arg;
        av[i] = const_cast<char*>(args[i].c_str());
    }
    av[2] = NULL;
    fd_file = createFile(".hamid", data);
    if (!(pipe(pipe_fd) > -1 && (pid = fork()) > -1)) {
        std::cerr << strerror(errno) << std::endl;
        exit(12);
    }
    if (pid == 0) {
        close(pipe_fd[0]);
        dup2(fd_file, STDIN_FILENO);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
        execve(av[0], av, NULL);
        std::cerr << strerror(errno) << std::endl;
        exit(12);
    } else {
        return parent_process(result, pipe_fd, pid);///////////////////
    }
    return false;
}


void get_body(Response * _response, std::string & result)
{
    std::string arg;
    std::istringstream ss(result);
       
    int pos0, pos1;
	std::cerr << "Content-type: |" << result << "|" << std::endl;

    pos0 = result.find("Content-Type");
    std::cerr << "\033[38;5;208msafe0 : "<< pos0 << "\033[0m" << std::endl;
    pos1 = result.find("\n");
    std::cerr << "\033[38;5;208msafe1 : "<< pos1 << "\033[0m" << std::endl;
    if (pos0 != -1)
    {
        _response->content_type = result.substr(pos0, pos1);
        std::cerr << "\033[38;5;208msafe2\033[0m" << std::endl;
        result.erase(pos0, pos1 + 1);
        std::cerr << "\033[38;5;208msafe3\033[0m" << std::endl;
    }
    else
        _response->content_type = "text/html";
        
    _response->body = result; 
    std::cerr << "\033[38;5;208msafe4\033[0m" << std::endl;
    _response->content_length = _response->body.size();
    std::cerr << "\033[38;5;208msafe5\033[0m" << std::endl;
    _response->status = 200;
}

void	_cgi( Request *_request, Response *_response , Server &_server )
{
    // printHeaders(_request->headers);
	std::string result, scriptPath, arg, _pwd;
    
    _pwd = getcwd(NULL, 0);
        // std::cerr << "\033[1;31m" << _pwd << "\033[0m" << std::endl;
    
    // std::cerr << "(*_request->cgi.begin()) =" << (*_request->cgi.begin())  << "| _request->path="<< _request->path <<  std::endl;
    if (!check_path_extension(_request->cgi , _request->path, scriptPath)) {
        std::cerr << "extansion" << std::endl;
        _response->body = "";
        return ;
    }
    update_env_for_cgi(_request, scriptPath, _server);
    if (!_request->body.empty())
    {
        std::cerr << "\033[1;35m**********\033[0m" << std::endl;
        exec_file_cgi(scriptPath, result, _pwd, _request->body);
    }
    else
        exec_cgi(scriptPath, result, _pwd);
	get_body(_response, result);
    
	// std::cerr << "Content-type: |" << _response->content_type << "|" << std::endl;
	// std::cerr << "execution output: |" << _response->body <<"|" << std::endl;
    
}
