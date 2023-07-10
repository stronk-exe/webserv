#include <iostream>
#include <string>

size_t findSubstring(const std::string& str, const std::string& subStr, size_t startPos) {
    return str.find(subStr, startPos);
}

int main() {
    std::string str = "/sc/hmoda/";
    std::string subStr = "/";
    size_t startPos = 3 + 1;  // Start searching from position 7

    size_t foundPos = findSubstring(str, subStr, startPos);
    if (foundPos != std::string::npos) {
        std::cout << "Substring found at position " << foundPos << std::endl;
    } else {
        std::cout << "Substring not found" << std::endl;
    }

    return 0;
}
