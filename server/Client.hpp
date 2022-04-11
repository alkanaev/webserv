/**
 * @author      : abaudot (aimebaudot@gmail.com)
 * @file        : Client
 * @created     : Wednesday Mar 16, 2022 16:48:54 CET
 */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "ServerBlock.hpp"
# include "Request.hpp"
# include "Response.hpp"

# include <fcntl.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <netinet/in.h>
# include <arpa/inet.h>

# define CLIENT_BUFFER_SIZE 4092
# define CLIENT_TIMEOUT 60

/* minimal Client class... */
class Client
{
	private:
		/* private data */
		ServerBlock			*_serv;
		Request				*_request;
		Response			*_response;
		struct sockaddr_in	_addr;
		struct timeval		_ping;
		socklen_t			_addrln;
		int					_fd;

	public:
		/* constructor */
		Client ( ServerBlock *serv, int ev_fd ):
			_serv(serv),
			_request(0),
			_response(0),
			_addr(),
			_addrln(),
			_fd(-1) {
				(void)_serv;
				_fd = accept(ev_fd, (struct sockaddr *)&_addr, &_addrln);
				if (_fd == -1)
					std::cerr << "accept() failed\n";
				if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1) {
					close(_fd);
					_fd = -1;
					std::cerr << "fcntl() failed\n";
				}
				gettimeofday(&_ping, NULL);
				std::cout << " [🐱] New Client: " << CYAN <<
					get_ip() << EOC;
			}

		/* destructor */
		~Client () {
			if (_fd != -1)
				close(_fd);
			if (_request)
				delete _request;
			if (_response)
				delete _response;
			std::cout << " [👻] " << CYAN << get_ip() << EOC;
		}

		/* getter */
		int		get_fd() const { return _fd; }
		bool	is_expired(time_t now) const {
			return ((now - _ping.tv_sec) > CLIENT_TIMEOUT); //one minutes.... test it pls
		}

		/* tmp */
		READ	read_request() {
			static char buffer[CLIENT_BUFFER_SIZE + 1];

			ssize_t n = recv(_fd, buffer, CLIENT_BUFFER_SIZE, 0);
			if (n == -1) {
				std::cerr << " recv(): failed\n";
				return (READ_ERROR);
			} else if (!n)
				return (READ_EOF);
			else {
				if (!_request) {
					_request = new Request(buffer, n);
					gettimeofday(&_ping, NULL); //maybe
				} else 
					_request->add_buffer(buffer, n);
				return (_read_status());
			}
		}

		/* tmp */
		READ	send_response() {
			if (_response) {
				if (_response->done())
					delete _response;
				else
					return (_response->send(_fd));
			}
			_response = new Response(_serv, _request); //not sure need to malloc
			_response->build();
			return (_response->send(_fd));
		}

		bool	_close() {
			if (_request) {
				bool state = _request->closed();
				delete _request;
				_request = 0;
				return (state);
			}
			return (true);
		}
	private: 
		/* private functions */
		READ	_read_status() {
			static std::string const asked[] = { " [🧡] GET", " [💙] POST", " [💔] DELETE", " [👽] UNSUPORTED"};

			if (!_request->header_ready()) {
				if (!_request->have_read_enought())
					return (READ_WAIT);
				else 
					if (!_request->read_header())
						return (READ_OK);
			// std::cou << _request->read_body() << std::endl;
			std::cout << " [🐱]: " << CYAN << get_ip() << EOCC
				<< asked[_request->get_method()] << "\n";
			}
			if (_request->get_method() == _POST && !_request->read_body())
				return (READ_WAIT);
#ifdef DEBUG
			_request->print();
#endif
			return (READ_OK);
		}

		std::string const get_ip() {
			getpeername(_fd, (struct sockaddr *)&_addr, &_addrln);
			return (inet_ntoa(_addr.sin_addr));
		}

};
#endif /* end of include guard CLIENT_HPP */
