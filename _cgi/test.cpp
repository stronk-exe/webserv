#include "../webserv.hpp" 


int main() {
    std::vector<Client> Clients;

    // Add instances of Client to the vector
    // Response b;
    Client a(4);
    std::cerr << "-----------------------" << std::endl;
    // Client b (a);
    Clients.push_back(a);
    std::vector<Client>::iterator it = Clients.begin();
    std::advance(it, 0);
    Clients.erase(it);
    // Clients.push_back(Client(4));

    // Access and manipulate the vector as needed

    return 0;

}