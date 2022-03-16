######################################################################
# @author      : abaudot (aimebaudot@gmail.com)
# @file        : Makefile
# @created     : Wednesday Mar 16, 2022 18:33:57 CET
######################################################################

CC=clang++
CFLAGS=-I. -Werror -Wextra -Wall -O2 -std=c++98


OBJ = main.o 

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean re fclean server

clean:
	rm -f *.o
fclean: clean
	rm -f server
re: fclean
	make server

