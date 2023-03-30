#include "webserv.hpp"

typedef struct s_server
{
	std::string port;
	std::string host;
	struct s_server *next;
}	t_server;

void _parser( std::string s )
{
	// std::cout << std::count(s.begin(), s.end(), "server {") << std::endl;
	// std::cout << s.find("server {") << std::endl;
	// int i =747;
	int count = 0;
	std::string str= "server {";
    size_t nPos = s.find(str, 0); 
	while (nPos != std::string::npos)
	{
		count++;
		nPos = s.find(str, nPos + str.size());
	}

	// number of servers we have
	std::cout << count << std::endl;

	// std::string _servers[count];
	int i=0;
	// int lock=0, c=0;
	// int temp=0;

	while (s[i])
	{
		int index = (s.substr(i, s.length())).find("server {")+9;
		// temp = index;
		// std::cerr << "indeeeeeeexxxxxxxxx " << s.substr(i, s.length()) << "--------" << std::endl;
		i += index;
		// temp = i;
		while (s[i] && s[i] != '}')
		{
			if (s[i] == '{')
			{
				i++;
				while (s[i] != '}' && s[i])
				{
					std::cout << s[i];
					i++;
				}
				i++;
			}
			// 	lock = 1;
			// if (s[index+1] == '}' && lock == 1)
			// 	lock = 0;
			// if (s[index] == '}' && lock == 0)
			// 	lock = 1;
			else
			{
				std::cout << s[i];
				i++;
			}
			// i++;
			// index++;
		}
		// if (temp != i)
		// 	c++;
		// std::cerr << "fin hna " << s.substr(i+1, s.length()) << std::endl;
		i++;
		// break;
		// std::cerr << "iiiiiiiiiiiiiii " << i << std::endl;

	}


	// allocating and constracting the servers
	// t_server _servers[count];
	// for (int i=0; i<count; i++)
	// {
	// 	std::cout << s << std::endl;
	// }

	// while (s[i])
	// {
	// 	// if (s[i] == '{')
	// 	// {
	// 	// 	while (s[i] && s[i] != '}')
	// 	// 	{
	// 	// 		std::cout << s[i];
	// 	// 		i++;
	// 	// 	}
	// 	// }
	// 	std::cout << s[i];
	// 	i++;
	// }
	// and the parsing begins!!
	// if (strcmp())
}