#include <iostream>

std::string update_uri(std::string  uri)
{
	std::string tmp;
	for (size_t i =0; i < uri.size(); i++)
	{
		if (!(uri[i] == '/' && uri[i + 1] == '/'))
			tmp += uri[i];
	}
	return tmp;
}

int main()
{
    std::cout << update_uri("//sc/hmoda/") << std::endl;
}