/**
 * @author      : abaudot (aimebaudot@gmail.com)
 * @file        : Client
 * @created     : Wednesday Mar 16, 2022 16:48:54 CET
 */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "serverBlock.hpp"
#include <fcntl.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <netinet/in.h>
# include <arpa/inet.h>

# define CLIENT_BUFFER_SIZE 4092
# define CLIENT_TIMEOUT 60
# define RESPONSE "HTTP/1.1 200 OK\r\n\r\n Hello Weeb World :D!!"

class Client
{
	private:
		/* private data */
		serverBlock			*_serv;

		/* need to implement this class */
		//request
		//response
		/* ----- */
		struct sockaddr_in	_addr;
		struct timeval		_ping;
		socklen_t			_addrln;
		int					_fd;

		//		std::string			_ip;

	public:
		enum READ_STATUS {
			READ_ERROR = 0,
			READ_EOF,
			READ_OK
		};

		/* constructor */
		Client ( serverBlock *serv, int ev_fd ):
			_serv(serv),
			_addr(),
			_addrln(),
			_fd(-1) {
				(void)_serv;
				_fd = accept(ev_fd, (struct sockaddr *)&_addr, &_addrln);
				if (_fd == -1) {
					std::cerr << "accept() failed" << std::endl;
				}
				if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1) {
					close(_fd);
					_fd = -1;
					std::cerr << "fcntl() failed" << std::endl;
				}
				gettimeofday(&_ping, NULL);
			}
		/* destructor */
		~Client () {
			if (_fd != -1)
				close(_fd);
		}

		/* getter */
		int		get_fd() const { return _fd; }
		bool	is_expired(time_t now) const {
			return ((now - _ping.tv_sec) > CLIENT_TIMEOUT);
		}

		/* tmp */
		int	read_request() {
			static char buffer[CLIENT_BUFFER_SIZE + 1];

			ssize_t n = recv(_fd, buffer, CLIENT_BUFFER_SIZE, 0);
			if (n == -1) {
				std::cerr << "recv(): failed\n";
				return (READ_ERROR);
			} else if (!n)
				return (READ_EOF);
			else {
				std::cout << "Readed: \n\r" << buffer << "\n\r" << "from: "
					<< get_ip() << " client\n";
				return (READ_OK);
			}
		}

		/* tmp */
		bool	send_response() {
			/*
			   if (resp)
			   delete resp;
			   resp = new Response(req);
			   */
			//		if (send(_fd, resp->toString(), resp->size(), 0) == -1) {
			if (send(_fd, RESPONSE, strlen(RESPONSE), 0) == -1) {
				std::cerr << "send(): failed\n";
				return (true);
			}
			return (true);
		}

			private: 
		/* private functions */
		std::string const get_ip() {
			getpeername(_fd, (struct sockaddr *)&_addr, &_addrln);
			return (inet_ntoa(_addr.sin_addr));
		}
};

#endif /* end of include guard CLIENT_HPP */

