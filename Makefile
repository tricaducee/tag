ifeq ($(DEBUG), debug)
	DFLAGS = -fsanitize=address -g3
endif
OBJDIR	= obj
NAME	= tag
FILES	= main.cpp Ids.cpp TagFile.cpp srch.cpp
OBJS	= $(strip $(patsubst %.cpp,$(OBJDIR)/%.o, $(FILES)))
FLAGS	= -std=c++20 -Wall -Werror -Wextra
CC		= g++

all: $(NAME)

$(OBJDIR):
	@mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	@echo Generating tags objects... $@
	@$(CC) $(FLAGS) -c $< -o $@

$(NAME): $(OBJS)
	@echo Compiling tags...
	@$(CC) $(FLAGS) $(DFLAGS) $(OBJS) -o $(NAME)
	@echo Done

clean:
	@echo Deleting objects...
	@rm -rf $(OBJDIR)

fclean: clean
	@echo Deleting executable...
	@rm -f $(NAME)

re:	fclean all

.PHONY: clean fclean re all