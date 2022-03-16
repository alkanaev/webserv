/**
 * @author      : abaudot (aimebaudot@gmail.com)
 * @file        : serverBlock
 * @created     : Wednesday Mar 16, 2022 13:52:24 CET
 */

#ifndef SERVERBLOCK_HPP
# define SERVERBLOCK_HPP

# include <string>
# include <iostream>
# include "unistd.h"

class serverBlock
{
	protected:
		/* private data */
		std::string const 	_host;
		std::string const 	_name;
		std::string const 	_root;
		int					_port;
	private:
		char buff[1000];

	public:
		/* constructors */
		serverBlock():
			_host("0.0.0.0"),
			_name("www.BestTeamEva.ru"),
			_root(getcwd(buff, 1000)),
			_port(8080) {}

		serverBlock( std::string const &host, std::string const &name,
				int const port ):
			_host(host),
			_name(name),
			_root(getcwd(buff, 1000)),
			_port(port){}

		serverBlock( serverBlock const &other ):
			_host(other._host),
			_name(other._name),
			_root(other._root),
			_port(other._port){}

		/* destructor */
		virtual ~serverBlock(){}

		/* getter */
		std::string const &get_name() const { return (_name); }
		std::string const &get_ip() const { return (_host); }
		std::string const &get_root() const { return (_root); }
		int get_port() const { return (_port); }

		/* setter */
		void	set_port( int const port ) { _port = port; }
};

#endif /* end of include guard SERVERBLOCK_HPP */

