/******************************************************************************
*             _/((		@author		: abaudot (aimebaudot@gmail.com)		  *
*    _.---. .'   `\		@created	: Thursday Mar 03, 2022 15:58:47 CET,	  *
*  .'      `     ^ T=	@filename	: server		 \)\_					  *
* /     \       .--'						        /    '. .---._            *
*|      /       )'-.			                  =P ^     `      '.          *
*; ,   __..-(   '-.)				               `--.       /     \         *
* \ \-.__)    ``--._)				               .-'(       \      |        *
*  '.'-.__.-.						              (.-'   )-..__>   , ;        *
*    '-...-'                                      (_.--``    (__.-/ /         *
*******************************************************************************/

#pragma once
#ifndef SERVER_HPP
# define SERVER_HPP

# include <sys/types.h>
# include <sys/socket.h>
# include <iostream>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <string.h>
# include <unistd.h>

# define	BLACK	"\033[1;30m"
# define	RED		"\033[1;31m"
# define	GREEN	"\033[1;32m"
# define	YELLOW	"\033[1;33m"
# define	BLUE	"\033[1;34m"
# define	CYAN	"\033[1;35m"
# define	PURPLE	"\033[1;36m"
# define	WHITE	"\033[1;37m"
# define	EOCC	"\033[0;0m"
# define	EOC		"\033[0;0m\n"

/*
class server
{
    public:
        server ();
        virtual ~server ();
    private:
};
*/

#endif /* end of include guard SERVER_HPP */

