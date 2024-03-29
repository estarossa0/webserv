# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: obouykou <obouykou@student.1337.ma>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/05/18 16:04:41 by arraji            #+#    #+#              #
#    Updated: 2021/06/30 11:56:43 by obouykou         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv
FLAGS = -Wall -Wextra -Werror

#FOLDERS
SRC_FOLDER = ./srcs
HEADER_FOLDER = ./includes
OBJECT_FOLDER = ./objects
LIBS =

#COLORS
RED = \033[0;31m
GREEN = \033[0;32m
GOLD = \033[0;33m
RESET = \033[0m

#FILES
SRC_FILES = main.cpp \
			Server.cpp \
			Connection.cpp \
			Webserv.cpp \
			Request.cpp \
			Response.cpp \
			ConfigParser.cpp \
			Location.cpp \
			ServerData.cpp \
			cgi.cpp \
			outputLogs.cpp \


OBJECT_FILES = $(SRC_FILES:.cpp=.o)
OBJECT_FILES := $(addprefix $(OBJECT_FOLDER)/, $(OBJECT_FILES))

# .PHONY = all clean fclean re credit

all: $(NAME)

$(NAME): $(OBJECT_FILES)
	@clang++ -g -I $(HEADER_FOLDER) $(OBJECT_FILES) $(LIBS) -o $@
	@echo
	@echo $(NAME)" created $(GREEN)successfully$(RESET)"

$(OBJECT_FOLDER)/%.o: $(SRC_FOLDER)/%.cpp $(HEADER_FOLDER)/*.hpp
	@(mkdir $(OBJECT_FOLDER) 2> /dev/null && echo "creating "$(OBJECT_FOLDER)" folder $(GREEN){OK}$(RESET)") || true
	@clang++ $(FLAGS) -g -I $(HEADER_FOLDER) -o $@ -c $< && echo "creating" $< "object $(GREEN){OK}$(RESET)"

clean:
	@(rm $(OBJECT_FILES) 2> /dev/null && echo "$(RED)deleting$(RESET): " $(OBJECT_FILES)) || true
	@(rm -r $(OBJECT_FOLDER) 2> /dev/null && echo "$(RED)deleting$(RESET): " $(OBJECT_FOLDER)) || true

fclean: clean
	@(rm $(NAME) 2> /dev/null && echo "$(RED)deleting$(RESET): " $(NAME)) || true

re: fclean $(NAME)
