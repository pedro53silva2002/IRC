NAME = ircserv
SRCS_DIR = ./srcs
SRCS = main.cpp Server.cpp myfuncs.cpp aux.cpp \
		commands/registration.cpp \
		commands/join.cpp \
		commands/privmsg.cpp \
		commands/part_quit.cpp \
		commands/topic.cpp \
		commands/mode.cpp \
		commands/invite.cpp \
		commands/kick.cpp 
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

clean:
	@$(RM) -r $(OBJS_DIR)
	@echo "$(NAME) OBJS cleaned!"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(NAME) cleaned!"

re: fclean all

run:
	@make; clear; ./ircserv 6667 pass

.PHONY: all fclean clean re
