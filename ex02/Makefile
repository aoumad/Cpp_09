NAME = PmergeMe

SRC		=	PmergeMe.cpp main.cpp

CC 		= c++

SANITIZE = -fsanitize=address
FLAGS = -std=c++98 -Wall -Wextra -Werror -g $(SANITIZE)

OBJS 	= $(SRC:.c=.o)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(SRC) -o $(NAME)

all: $(NAME) $(OBJS)

%.o: %.c
		$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(NAME)

fclean: clean

re: fclean all

.PHONY: all clean fclean re