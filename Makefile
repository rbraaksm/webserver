# Colours
GREEN		= \033[0;92m
RED			= \033[0;31m
END			= \033[0;0m

NAME 		= webserv

CXX 		= clang++
CXXFLAGS 	+= -Wall -Werror -Wextra -std=c++98
LDFLAGS 	+=

OBJDIR 		= obj
SRCDIR 		= ./srcs/

FILES = 	main \
			srcs/webserver \
			srcs/locationContext \
			srcs/fileDescriptors \
			srcs/response \
			srcs/server \
			srcs/headerHandler \
			srcs/requestBuf \
			srcs/client \
			srcs/Utils/fileUtils \
			srcs/Utils/manipulateUtils \
			srcs/Utils/parseUtils \
			srcs/Utils/getInfoUtils \
			srcs/Utils/headerUtils \
			srcs/Utils/Base64 \

SRC = $(addsuffix .cpp, $(FILES))
OBJ = $(SRC:%.cpp=$(OBJDIR)/%.o)

C_MESSAGE = "Building objects... %-33.33s\r"
MESSAGE = "\nHow to run: execute$(GREEN) ./webserv$(END) or type$(GREEN) './webserv [configFile]'$(END)\n"

all: $(NAME)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p '$(@D)'
	@printf $(C_MESSAGE) $@
	@$(CXX) -c $(CXXFLAGS) $< -o $@

$(NAME): $(OBJ)
	@echo "\n\nExecutable build: $(GREEN) $(NAME) $(END)\r"
	@$(CXX) $^ $(CXXFLAGS) $(LDFLAGS) -o $(NAME)
	@printf $(MESSAGE)

clean:
	@echo "Removing obj directory $(RED)$(OUT_DIR) $(END)"
	@rm -rf $(OBJDIR)
	@rm -rf serverFiles/www/temporaryFiles/*

fclean: clean
	@echo "Removing executable $(OUT_DIR)$(RED) $(NAME) $(END)"
	@rm -rf $(NAME)

re: fclean all

run: all
	@./webserv confFiles/valid/default.conf

siege:
	@./webserv confFiles/valid/siege.conf

.PHONY: clean fclean re all
