#include <cerrno>
#include <iostream>
#include <cstdio>

void get_arrgs_path(std::string path)
{
    char **av; 
    std::string arg;
    std::istringstream ss(path);
    
    if (av = malloc(2, ma))
        ; 
    for (int i = 0; i < 2; i++) {
        std::getline(ss, arg, ' ');
        av[i] = arg.c_str();
    }
    av[3] = NULL;
}

int main()
{
    char *av[3];

    std::string path = "cho wahd";
    
    get_arrgs_path(av, path);
    for (int i = 0; i < 2; i++) 
        std::cout << av[i] << " ";

 }

// int main(int ac, char *av[]) {
//     FILE *pipe;
//     char buffer[128];

//     if (ac != 2)
//         return 0;
//     // Open a pipe and execute the command
//     pipe = popen(av[1], "r");
//     if (pipe == NULL) {
//         perror("pipe");
//         return 1;
//     }
//     std::string result;
//     // Read the command output line by line
//     while (fgets(buffer, sizeof(buffer), pipe) != NULL) 
//         result += buffer;

//     // Close the pipe
//     pclose(pipe);
    
//     std::cout << result << std::endl;

//     return 0;
// }

// #include <cstdio>
// #include <cstring>

// int main() {
//     // Delete a file
//     const char* filename = "path/to/file.txt";
//     if (std::remove(filename) != 0) {
//         std::perror("Error deleting the file");
//         return 1;
//     }
//     else {
//         std::puts("File deleted successfully");
//     }

//     // Delete a directory
//     const char* dirname = "path/to/directory";
//     if (std::rmdir(dirname) != 0) {
//         std::perror("Error deleting the directory");
//         return 1;
//     }
//     else {
//         std::puts("Directory deleted successfully");
//     }

//     return 0;
// }