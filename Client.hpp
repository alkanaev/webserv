/**
 * @author      : abaudot (aimebaudot@gmail.com)
 * @file        : Client
 * @created     : Wednesday Mar 16, 2022 16:48:54 CET
 */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "serverBlock.hpp"
# include "Request.hpp"

# include <fcntl.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <netinet/in.h>
# include <arpa/inet.h>

# define CLIENT_BUFFER_SIZE 4092
# define CLIENT_TIMEOUT 60
# define RESPONSE "HTTP/1.1 200 OK\r\n\r\n <HTML>\n<HEAD>\n<TITLE>Your Title Here</TITLE>\n""</HEAD>\n<BODY BGCOLOR=\"FFFFFF\">\n<CENTER><IMG SRC=\"clouds.jpeg\" ALIGN=\"BOTTOM\"> </CENTER>\n<HR>\n<a href=\"http://somegreatsite.com\">Link Name</a>\nis a link to another nifty site\n<H1>This is a Header</H1>\n<H2>This is a Medium Header</H2>\nSend me mail at <a href=\"mailto:support@yourcompany.com\">\nsupport@yourcompany.com</a>.\n<P> This is a new paragraph!\n<P> <B>This is a new paragraph!</B>\n<BR> <B><I>This is a new sentence without a paragraph break, in bold italics.</I></B>\n<HR>\n</BODY>\n</HTML>\n"

/* minimal Client class... */
class Client
{
	private:
		/* private data */
		serverBlock			*_serv;

		/* need to implement this class */
		//response
		/* ----- */
		Request				*_request;
		struct sockaddr_in	_addr;
		struct timeval		_ping;
		socklen_t			_addrln;
		int					_fd;

	public:
		/* constructor */
		Client ( serverBlock *serv, int ev_fd ):
			_serv(serv),
			_request(0),
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
			}

		/* destructor */
		~Client () {
			if (_fd != -1)
				close(_fd);
			if (_request)
				delete _request;
		}

		/* getter */
		int		get_fd() const { return _fd; }
		bool	is_expired(time_t now) const {
			return ((now - _ping.tv_sec) > CLIENT_TIMEOUT);
		}

		/* tmp */
		READ	read_request() {
			static char buffer[CLIENT_BUFFER_SIZE + 1];

			ssize_t n = recv(_fd, buffer, CLIENT_BUFFER_SIZE, 0);
			if (n == -1) {
				std::cerr << "recv(): failed\n";
				return (READ_ERROR);
			} else if (!n)
				return (READ_EOF);
			else {
				if (!_request) {
					_request = new Request(buffer);
					//change ping time
				} else 
					_request->add_buffer(buffer);
				return (_read_status());
			}
		}

		/* tmp */
		bool	send_response() {
			/* so far default response.... */
			if (send(_fd, RESPONSE, strlen(RESPONSE), 0) == -1) {
				std::cerr << "send(): failed\n";
				return (true);
			}
			return (true);
		}

			private: 
		/* private functions */
		READ	_read_status() {
			if (!_request->header_ready()) {
				if (!_request->have_read_enought())
					return (READ_WAIT);
				else 
					if (!_request->read_header())
						return (READ_OK);
			}
			if (_request->get_method() == _POST && !_request->read_body())
				return (READ_WAIT);
			//////// debug /////////
			_request->print();
			//////////////////////
			return (READ_OK);
		}

		std::string const get_ip() {
			getpeername(_fd, (struct sockaddr *)&_addr, &_addrln);
			return (inet_ntoa(_addr.sin_addr));
		}
};

#endif /* end of include guard CLIENT_HPP */
