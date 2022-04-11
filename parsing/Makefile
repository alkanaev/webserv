NAME = parsingNGINX

all:
	clang++ -std=c++98 -I../block -I ../Server -I. main.cpp Conf_main.cpp Conf_checkers.cpp Conf_init.cpp Conf_tools.cpp Conf_print.cpp -o $(NAME)
clean:
	rm -f main.o Config.o

fclean: clean
	rm -f $(NAME)

re: fclean all
