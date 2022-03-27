NAME = parsingNGINX
CC=c++
FLAGS = -std=c++98
SRC = Config.cpp main.cpp
OBJ = $(SRC:.cpp=.o)
%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
