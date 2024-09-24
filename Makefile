# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lboulang <lboulang@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/06 17:01:05 by lboulang          #+#    #+#              #
#    Updated: 2024/05/22 18:36:18 by lboulang         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#                                  SOURCES                                     #
################################################################################

SRC_FOLDER  = 	./Srcs/
SRC_FILES	=	main.cpp\
				Server.cpp\
				Webserv.cpp\
				Request.cpp\
				Response.cpp\
				AutoIndex.cpp\
				CGI.cpp\
				Utils.cpp\
				Parsing/CheckInstructions.cpp\
				Parsing/ParseBlocks.cpp\
				Parsing/ParseConfFile.cpp\
				Config.cpp

################################################################################
#                                  OBJETS                                      #
################################################################################

OBJS_DIR	= objs
OBJS		= $(addprefix $(OBJS_DIR)/, $(addsuffix .o, $(basename $(SRC_FILES))))

################################################################################
#                                COMPILATION                                   #
################################################################################

CC			= c++
CFLAGS		= -g3 -std=c++98 -Wall -Wextra -Werror -I Includes -MMD -MP

################################################################################
#                                  MAKEFILE                                    #
################################################################################

NAME = webserv

all : $(NAME)

$(NAME) : $(OBJS_DIR) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJS_DIR) :
	mkdir -p $(OBJS_DIR)
	mkdir -p $(OBJS_DIR)/Parsing

-include $(OBJS:%.o=%.d)

$(OBJS) : $(OBJS_DIR)/%.o : $(SRC_FOLDER)%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -rf $(NAME)
	
re : fclean all

.PHONY: all clean fclean re