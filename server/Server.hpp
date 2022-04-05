/**
 * @author      : abaudot (aimebaudot@gmail.com)
 * @file        : Server
 * @created     : Wednesday Mar 16, 2022 14:06:16 CET
 */

#ifndef VHOST_HPP
# define VHOST_HPP

# include "ServerBlock.hpp"

# include <fcntl.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <arpa/inet.h>

# define MAX_CONNS 4092

class Server: public ServerBlock
{
	private:
		/* private data */
		int _fd;
	public:
		/* constructor(s) */
		explicit Server ( ServerBlock const &serv ):
			ServerBlock(serv), 
			_fd(-1) {
				_create_socket();
				_set_noblock();
				_set_sockopt();
				_bind_socket();
				_listen_socket();
			}

		/* destructor */
		~Server(){
			if (_fd != -1)
				close(_fd);
		}
		/* getter */
		int	get_fd() { return (_fd); }

	private:
		/* private methodes */
	void	_create_socket() {
		_fd = socket(AF_INET, SOCK_STREAM, 0); //TCP socket
		if (_fd == -1)
			throw std::runtime_error("socket() failed");
	}

	void	_set_noblock() {
		if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1) // set socket as non-bloking
			throw std::runtime_error("fcntl() failed");
	}

	void	_set_sockopt() {
		int _true = 1;
		if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &_true, sizeof(_true)) == -1)
			throw std::runtime_error("setsockopt() failed");
	}

	void	_bind_socket() { // assign address to socket
		struct sockaddr_in addr;

		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = inet_addr(ip.c_str());
		if (bind(_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
			throw std::runtime_error("bind() failed");
	}

	void	_listen_socket() {
		if (listen(_fd, MAX_CONNS) == -1)
			throw std::runtime_error("listen() failed");
		std::cout << " [📡] " << get_name()<< " bound on "
			<< GREEN << ip << EOCC << ":" << RED << get_port() << EOC;
	}
};

#endif /* end of include guard VHOST_HPP */

