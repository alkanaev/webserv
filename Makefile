######################################################################
# @author      : abaudot (aimebaudot@gmail.com)
# @file        : Makefile
# @created     : Monday Mar 28, 2022 19:27:22 CEST
######################################################################

NAME=ruServ

CC=clang++
INCLUDES= -I block -I parsing -I server
CFLAGS= -std=c++98 -Wall -Werror -Wextra -g3 #debug

OBJ = main.o parsing/Config.o server/Response.o

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDES)

$(NAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(INCLUDES)

all: $(NAME)

.PHONY: clean re fclean server

clean:
	rm -f *.o
	rm -f parsing/*.o
	rm -f server/*.o
fclean: clean
	rm -f $(NAME)
re: fclean
	make $(NAME)
