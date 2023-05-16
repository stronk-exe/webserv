#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <unistd.h>
#include <stdlib.h>

int main(int ac, char **avs, char **env)
{
    char *av[3];
    char buffer[100];

    av [0] = "/usr/bin/php";
    av [1] = "/Users/mait-jao/Project/webserv/cgi-bin/php.cgi";
    av [2] = NULL;
     int pipe_fd[2];
    pid_t pid;

    if (pipe(pipe_fd) == -1) {
        printf("%s\n", strerror(errno)) ;
        exit(12);
    }
     pid = fork();
    if (pid == -1) {
        printf("%s\n", strerror(errno)) ;
        exit(12);
    }
    if (pid == 0) {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
        execve(av[0], av, env);
        printf("%s\n", strerror(errno)) ;
        exit(12);
    } else {

        close(pipe_fd[1]);
        int nbytes;
        while ((nbytes = read(pipe_fd[0], buffer, sizeof(buffer))) > 0)
            printf("%s", buffer);
        close(pipe_fd[0]);

        // int status;
        // waitpid(pid, &status, 0);
        // if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        //     std::cout << result << std::endl;
        // else
        //     std::cout <<"Script execution failed."<< std::endl;
    }
}1