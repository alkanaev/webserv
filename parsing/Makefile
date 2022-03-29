NAME = parsingNGINX

all:
	clang++ -std=c++98 -I../block -I ../Server -I. main.cpp Config.cpp -o $(NAME)
clean:
	rm -f main.o Config.o

fclean: clean
	rm -f $(NAME)

re: fclean all
