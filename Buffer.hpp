/******************************************************************************
*             _/((		@author		: abaudot (aimebaudot@gmail.com)		  *
*    _.---. .'   `\		@created	: Thursday Mar 03, 2022 18:13:56 CET,	  *
*  .'      `     ^ T=	@filename	: Buffer		 \)\_					  *
* /     \       .--'						        /    '. .---._            *
*|      /       )'-.			                  =P ^     `      '.          *
*; ,   __..-(   '-.)				               `--.       /     \         *
* \ \-.__)    ``--._)				               .-'(       \      |        *
*  '.'-.__.-.						              (.-'   )-..__>   , ;        *
*    '-...-'                                      (_.--``    (__.-/ /         *
*******************************************************************************/

#pragma once

#ifndef BUFFER_HPP
# define BUFFER_HPP

#include "server.hpp"
# include <algorithm>
# include <string>
# include <vector>

# define BUFFSZ 65536

class Buffer
{
    public:
        Buffer (): first(), last() {}
        virtual ~Buffer (){}

		int			findChar(std::vector<char> &str, char const c);
		bool		empty();
		void		resetBuffer();
		bool		recv(int const fd);

    private:
        /* private data */
		char	b[BUFFSZ + 1];
		int		first;
		int		last;
};

#endif /* end of include guard BUFFER_HPP */

