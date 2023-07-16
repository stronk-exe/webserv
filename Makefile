# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mait-jao <mait-jao@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/05/11 11:16:24 by ael-asri          #+#    #+#              #
#    Updated: 2023/05/16 15:28:50 by mait-jao         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

INC = webserv.hpp

SRCS =  main.cpp \
		_config_parser/parsing.cpp\
		_socket/socket.cpp\
		_methodes/methods.cpp\
		_request/request.cpp\
		_response/response.cpp\
		_cgi/cgi.cpp

OBJS = $(SRCS:.cpp=.o)


CPP = c++
FLAGS =  -Wall -Wextra -Werror -std=c++98

#colors

YELLOW = $(shell echo "\033[1;93m")
PURPLE = $(shell echo "\033[1;95m")
GREEN = $(shell echo "\033[1;92m")
WHITE = $(shell echo "\033[1;97m")
BLUE = ${shell echo "\033[1;94m"}
RED = $(shell echo "\033[1;91m")

all: $(NAME)

$(NAME): ${OBJS} ${INC}
	@ echo "\n$                             ${YELLOW}}}}.....${GREEN}!Compile :-> ${NAME} <-: Successe!${YELLOW}.....{{{${WHITE}                           \n"
	@rm -rf _cgi/cgi_utils/* 
	@rm -rf _cgi/cgi_utils/.h* 
	@$(CPP) $(FLAGS) ${OBJS} -o $@

%.o : %.cpp
	@ echo "${BLUE}Generating Webserv objects... ${PURPLE}" $@ "${WHITE}"
	$(CPP) $(FLAGS) -c $< -o $@


clean:
	@rm -rf _cgi/.h* 
	@ echo "${YELLOW}-> ${PURPLE}Delete the object files.....${WHITE}"
	rm -f $(OBJS)

fclean: clean
	@ echo "${YELLOW}-> ${RED}fclean : Deleting executable done.${WHITE}"
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
