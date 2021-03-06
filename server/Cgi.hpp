/******************************************************************************
*             _/((		@author		: abaudot (aimebaudot@gmail.com)		  *
*    _.---. .'   `\		@created	: Sunday Apr 03, 2022 14:44:01 CEST,	  *
*  .'      `     ^ T=	@filename	: Cgi            \)\_					  *
* /     \       .--'						        /    '. .---._            *
*|      /       )'-.			                  =P ^     `      '.          *
*; ,   __..-(   '-.)				               `--.       /     \         *
* \ \-.__)    ``--._)				               .-'(       \      |        *
*  '.'-.__.-.						              (.-'   )-..__>   , ;        *
*    '-...-'                                      (_.--``    (__.-/ /         *
*******************************************************************************/

#ifndef CGI_HPP
# define CGI_HPP

# include "Request.hpp"

# include <map>
# include <unistd.h> /* fork pipe.. */
# include <fcntl.h> /* open */
# include <sys/wait.h> //?
# include <signal.h> /* kill */
# include <sstream> /*stringstream*/
# include <fstream> /* offstream ifstream */

# define _CGI_TIMEOUT		5
# define _CGI_TICKS_US		100

class Cgi
{
	public: /* type */
		typedef std::map<std::string, std::string> HeadersObject; /* multimap ? */
		typedef std::pair<std::string, std::string>		HeaderPair;
		typedef std::map<std::string, std::string> 		EnvVarObject;
	private: /* data */

		EnvVarObject		_env;
		HeadersObject		_headers;

		std::string const	_bin_path;
		std::string const	_file_path;
		std::string const	_out;
		std::string			_body;
		int					_fds[2];
		int					_out_fd;
		METHODS				_method;
		bool				_timeout;

	public: /* methods */
		/* construnctor */
		Cgi ( std::string const &bin_path, std::string const &file_path,
				std::string const &query, METHODS const meth ):
			_bin_path(bin_path),
			_file_path(file_path),
			_out(_randstr(16)), /* give him a name (overkill right now */
			_method(meth),
			_timeout(0) {
				_env["QUERY_STRING"] = query;
			}

		/* destructor */
		~Cgi () { remove(_out.c_str()); } /* delet tmp out file */

		/* getter */
		std::string const &get_output() const { return _body; }
		bool timed_out() const { return _timeout; }
		HeadersObject::const_iterator
			Hbegin() const { return _headers.begin(); }
		HeadersObject::const_iterator
			Hend() const { return _headers.end(); }

		bool
			setup( std::string const &request,
					Request::Headers const &head ) {
				if (pipe(_fds) == -1)
					return (false);
				if (write(_fds[1], request.c_str(), request.size()) < 0)
					return (false);
				_env["CONTENT_LENGTH"] = _toString(request.size()); //
				_out_fd = open(_out.c_str(),
						O_RDWR | O_CREAT, S_IWRITE | S_IREAD);
				if (_out_fd == -1)
					return (false);
				close(_fds[STDOUT_FILENO]);
				_setup_env(head);
				return (true);
			}

		bool	run() {

			std::cout << " [???????] Running cgi Script: "
				<< YELLOW << _file_path << EOC;
			pid_t	bin_pid = fork();
			if (bin_pid < 0) {
				std::cerr << "fork() failed\n";
				close(_out_fd);
				return (false);
			}
			if (bin_pid == 0)
				_exec_bin();
			if (!_wait_bin(bin_pid))
				return (false);
			return (_extract_response());
		}

	private: /* private methods */

		/* https://en.wikipedia.org/wiki/Common_Gateway_Interface */
		void	_setup_env( Request::Headers const &headers ) {

		Request::Headers::const_iterator it = headers.begin();
		for (; it != headers.end(); ++it) {
			if (it->first == "content-type")
				_env["CONTENT_TYPE"] = it->second;
			_env["HTTP_" + _header_to_cgi(it->first)] = it->second;
			}
			_env["GATEWAY_INTERFACE"] = "CGI/1.1";
			_env["SCRIPT_FILENAME"] = _file_path;
			_env["REDIRECT_STATUS"] = "200";

			switch(_method) {
				case _GET: _env["REQUEST_METHOD"] = "GET"; break ;
				case _POST: _env["REQUEST_METHOD"] = "POST"; break;
				default: _env["REQUEST_METHOD"] = "UNKNOWN";
			}
		}

		void _exec_bin() {
			dup2(_fds[STDIN_FILENO], STDIN_FILENO);
			dup2(_out_fd, STDOUT_FILENO);
			close(_fds[STDIN_FILENO]);

			char	*argv[] = {
				const_cast<char*>(_bin_path.c_str()),
				const_cast<char*>(_file_path.c_str()), 0 // why
			};

			if (execve(argv[0], argv, _dump_env()) == -1) {
				std::cerr << " bad execve\n";
				exit(EXIT_FAILURE); //mem leak but ok cause exit (_exit ?)
			}
		}
		char	**_dump_env() {
			char **ret = (char**)malloc(sizeof(char*) * (_env.size() + 1));
			ret[_env.size()] = 0;

			EnvVarObject::const_iterator it = _env.begin();
			for (std::size_t i = 0; it != _env.end(); ++it, i++) {
				std::string payload = it->first + "=" + it->second;
				ret[i] = strdup(payload.c_str());
			}
			return (ret);
		}
		/* wait bin process du return... checking if the process timeout */
		bool	_wait_bin( int const bin_pid ) {
			struct timeval	current_time;
			struct timeval	start_time;
			int				pid, state;

			gettimeofday(&start_time, NULL);
			while (true) {
				pid = waitpid(bin_pid, &state, WNOHANG);
				if (pid == -1) {
					//if (_timeout) break ;
					std::cerr << "waitpid() failed\n";
					close(_out_fd);
					return (false);
				}

				usleep(_CGI_TICKS_US); //
				if (pid && WIFEXITED(state))
					break ;

				gettimeofday(&current_time, NULL);
				if (current_time.tv_sec >
						start_time.tv_sec + _CGI_TIMEOUT) {
					//					if (current_time.tv_usec < start_time.tv_usec)
					//						continue ; // ???????
					std::cerr << "time out\n";
					kill(bin_pid, SIGKILL);
					_timeout = true;
					break ;
				}
			}
			close(_out_fd);
			return (!_timeout);
		}

		/* make response from cgi result */
		bool	_extract_response() {
			std::ifstream	file(_out.c_str());
			if (!file.is_open())
				return (false);
			std::string data((std::istreambuf_iterator<char>(file)),
					std::istreambuf_iterator<char>());
			file.close();
//			std::cout << "DATA: \n" << data << "\n ---------------\n";
			return (_parse_response(data));
		}

		/*get header for the response*/
		bool	_parse_response( std::string const &data ) {
			size_t sep_pos = data.find("\r\n\r\n"); /*header*/
			if (sep_pos == std::string::npos)
				return (false); /* no header => false ? */

			std::string headers = data.substr(0, sep_pos);
			_body = data.substr(sep_pos + 4); /* after \r\n\r\n is body */

			size_t nl_pos = 0;
			while (1) {
				size_t tmp = headers.find("\r", nl_pos); //"\n\rj
				if (tmp == std::string::npos)
					break ;
				sep_pos = headers.find(":", nl_pos);
				std::string const key = headers.substr(nl_pos, sep_pos - nl_pos);
				std::string const value =
					headers.substr(sep_pos + 2, tmp - (nl_pos + key.size() + 1));
				/*
				//// DEBUG ///
				std::cout << "key: " << key << "\n";
				std::cout << "value: " << value << "\n";
				///////
				*/
				_headers.insert(HeaderPair(key, value));
				nl_pos = tmp + 2;
			}
			return (true);
		}

		/* ------- UTILLITY --------- */
		static inline std::string _header_to_cgi( std::string in ) {
			for (size_t i = 0; i < in.size(); i++) {
				in[i] = toupper(in[i]);
				if (in[i] == '-')
					in[i] = '_';
			}
			return (in);
		}
		std::string _randstr(const int len) {
			static const char alphanum[] =
				"0123456789"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz";
			struct timeval tv;
			gettimeofday(&tv, NULL);
			srand(tv.tv_usec);
			std::string ret;
			ret.reserve(len);

			for (int i = 0; i < len; ++i)
				ret += alphanum[rand() % (sizeof(alphanum) - 1)];
			return (ret);
		}
		std::string	_toString(size_t num) { //again
			std::stringstream ss;

			ss << num;
			return (ss.str());
		}
};
#endif /* end of include guard CGI_HPP */

