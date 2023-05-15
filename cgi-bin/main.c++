#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

# include <fstream>
#include <cerrno>


int main( int ac, char *av[]) {
    int pipe_fd[2];
    pid_t pid;
    char buffer[100];

    if (ac != 2)
        return 0;
    // Create a pipe
    if (pipe(pipe_fd) == -1) {
        strerror(errno);
        return 1;
    }
    std::string str;
    std::ifstream file(av[1]);
    std::getline(file, str);
    str.erase(str.begin(), str.begin() + 2);
    std::cout << str << std::endl;
    if (access(str.c_str(), X_OK))
        return 1;
    // Fork a new process
    av[0] = &str[0];
    pid = fork();
    if (pid == -1) {
        strerror(errno);
        return 1;
    }

    if (pid == 0) {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
        execve(av[0], av, NULL);
        strerror(errno);
        return 1;
    } else {
        std::string result;
        close(pipe_fd[1]);
        int nbytes;
        while ((nbytes = read(pipe_fd[0], buffer, sizeof(buffer))) > 0)
            result.append(buffer, nbytes);
        close(pipe_fd[0]);

        int status;
        waitpid(pid, &status, 0);

        // Check if the child process exited successfully
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
            std::cout << result << std::endl;
        else
            std::cout <<"Script execution failed."<< std::endl;
    }

    return 0;
}