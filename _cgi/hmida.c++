#include <iostream>
#include <string>
#include <sstream>

bool isValidIPAddress(const std::string& ipAddress) {
    std::istringstream iss(ipAddress);
    int num;
    char dot;
    int count = 0;

    while (iss >> num >> dot) {
        if (num >= 0 && num <= 255 && dot == '.') {
            count++;
        }
        else {
            return false;
        }
    }

    return count == 4 && !(iss >> num);
}

int main() {
    std::string ipAddress;
    std::cout << "Enter an IP address: ";
    std::cin >> ipAddress;

    if (isValidIPAddress(ipAddress)) {
        std::cout << "Valid IP address." << std::endl;
    }
    else {
        std::cout << "Invalid IP address." << std::endl;
    }

    return 0;
}
