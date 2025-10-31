NAME = ircserv
SRCS_DIR = ./srcs
SRCS = main.cpp Server.cpp myfuncs.cpp Client.cpp
OBJS_DIR = ./objs
OBJS = $(addprefix $(OBJS_DIR)/,$(SRCS:.cpp=.o))
CXX = c++
CXXFLAGS = -std=c++98 -g #-Wall -Wextra -Werror 
INCLUDES = -I includes
RM = rm -f

all: $(NAME)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJS)
	@echo "$(NAME) compiling..."
	@$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS) $(INCLUDES)
	@echo "$(NAME) ready!"

valgrind: 
	/usr/bin/valgrind --leak-check=full -s --show-leak-kinds=all ./$(NAME) 6667 pass

clean:
	@$(RM) -r $(OBJS_DIR)
	@echo "$(NAME) OBJS cleaned!"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(NAME) cleaned!"

re: fclean all

run: all
	@./ft_irc

.PHONY: all fclean clean re
