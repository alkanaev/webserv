/******************************************************************************
*             _/((		@author		: abaudot (aimebaudot@gmail.com)		  *
*    _.---. .'   `\		@created	: Thursday Mar 24, 2022 15:27:10 CET,	  *
*  .'      `     ^ T=	@filename	: Response		 \)\_					  *
* /     \       .--'						        /    '. .---._            *
*|      /       )'-.			                  =P ^     `      '.          *
*; ,   __..-(   '-.)				               `--.       /     \         *
* \ \-.__)    ``--._)				               .-'(       \      |        *
*  '.'-.__.-.						              (.-'   )-..__>   , ;        *
*    '-...-'                                      (_.--``    (__.-/ /         *
*******************************************************************************/

#ifndef RESPONSE_HPP
# define RESPONSE_HPP
# include "Request.hpp"
# include <sys/stat.h> /* stat function */

class Response
{
	typedef std::map<std::string, std::string>		Headers;

	private:
	/* private data */
	Headers		_headers;

	serverBlock	*_server;
	Request 	*_request;

	std::string _body;
	std::string _page;

	int			_status;
	//maby I will end up adding a LocationBlock

	public:
	/* constructor */
	Response ( serverBlock *serv, Request *request ):
		_server(serv),
		_req(request),
		_status(request->get_code()){}

	explicit Response ( int const code )
		_server(0),
		_req(0),
	:	_status(code){}

	/* destructor */
	~Response ();

	void	build () {
		if (_request && _status < BAD_REQUEST)
			_construct_body();
		if (_status >= BAD_REQUEST) {
			if (_request) {
				_body = _server->get_error_page(_status,
						_request->get_host(), _request->get_uri());
				if (_body == "")
					_body = generate_status_page(_status);
			} else {
				_body = generate_status_page(_status);
			}
		}
		_page  = _construct_header() + _body + "\r\n";
	}
	private: /* private functions */
	void	_construct () {
		/////////
		const LocationBlock  lblock = _server->retrive_location(
			_request->get_host(), _request->get_uri());
		/////////

		if (lblock->get_body_limit() < _request->get_raw().size()) {// if !lb
			_status = PAYLOAD_TOO_LARGE;
			return ;
		}

		METHODS const meth = _request->get_method();
		if (meth == _GET) //switch
			_get(lblock);
		else if (meth == _POST)
			_post(lblock);
		else if (meth == _DELETE)
			_delete(lblock);
		else
			_status = METHOD_NOT_ALLOWED;
	}

	std::string _construct_header () {
		if (!_request || (_request &&_request->closed()))
			_headers["Connection"] = "closed";
		else
			_headers["Connection"] = "keep-alive";

		if (_headers["Content-Type"] == "") {
			if (_status != HTTP::OK)
				_headers["Content-Type"] = get_mime_type(".html");
			else if (_req)
				_headers["Content-Type"] = get_mime_type(_req->get_uri());
			else
				_headers["Content-Type"] = get_mime_type("/");
		}
		_headers["Content-Length"] = _toString(_body.size());
		_set_header_date();
		_headers["Server"] = WEBSERV_SERVER_VERSION;
		#ifdef WEBSERV_BUILD_COMMIT
			_headers["Server"] += WEBSERV_BUILD_COMMIT;
		#endif

		std::stringstream head;
		head << "HTTP/1.1 " << _status << " " << resolve_code(_status) << "\r\n";

		std::string headers;
		Headers::const_iterator hit = _headers.begin();
		for (; hit != _headers.end(); ++hit)
			headers += hit->first + ": " + hit->second + "\r\n";

		Cookies::const_iterator cit = _cookies_to_set.begin();
		for (; cit != _cookies_to_set.end(); ++cit) {
			if (cit->first == "Set-Cookie")
				headers += cit->first  + ": " + cit->second + "\r\n";
			else
				headers += "Set-Cookie: " + cit->first + "=" + cit->second + "\r\n";
		}
		return head.str() + headers + "\r\n";
	}
	/* ----------------- GET ------------------------- */
	void	_get( LocationBlock const *lblock ) {
		std::string const path = _build_method_path(lblock, _GET, false);
		if (path == "")
			return ;

		if (block->get_redirection() != "")
			return ((void)_do_redirection(block)); //
		_get_file(lblock, path);
	}

	std::string _build_method_path( LocationBlock const *lblock,
		METHODS  method, bool check_upload_pass ) {
		std::string path;

		if (lblock->is_allowed(method) == false) {
			_status = METHOD_NOT_ALLOWED;
			return ("");
		}
		if (check_upload_pass && lblock->get_upload_pass() != "")
			path = lblock->get_upload_pass();
		else
			path = lblock->get_root();
		return (path + _request->get_uri());
	}

	bool	_get_file( LocationBlock const *lblock,
			std::string const &path ) {

		/*
		 * need to handel cgi case => return 
		*/
		if (path[path.size() - 1] == '/') /* is a dir */
			return (_get_dir(lblock, path)); //////// return val

		/* error handeling */
		errno = 0;
		struct stat st; /* cf man 2 stat */
		if (stat(path.c_str(), &st) == -1) {
			_status = INTERNAL_SERVER_ERROR;
			if (errno == ENOENT || errno == ENOTDIR) /* file do not exit */
				_status = NOT_FOUND;
			return (true);
		}

		/*type of file: 0170000 */
		if ((st.st_mode & S_IFMT) == S_IFDIR)
			return (_get_dir(lbock, _request->get_uri()));
		_body = _get_file_content(path);
		return (true);
	}

	bool	_get_dir( LocationBlock const *lblock, std::string const &path ) {

		std::vector<struct dirent> files;
		if (!_dump_files_dir(path, &files)) //why not ref ? //
			return (false);
		if (_get_index(lblock, path, files))
			return (true);
		if (lblock->is_autoindex() == true)
			return (_get_autoindex(files, path));
		_status = NOT_FOUND;
		return (false); /* usage ? */
	}

	std::string _get_file_content( std::string const &path ) {
		int fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
		if (fd == -1) {
			if (errno == EACCES)
				_status = FORBIDDEN;
			_status = INTERNAL_SERVER_ERROR;
			return ("");
		}

		ssize_t n;
		char buf[1024];
		std::stringstream ss;
		while ((n = read(fd, buf, sizeof(buf))) > 0)
			ss.write(buf, n);
		close(fd);
		return (ss.str());
	}

};

#endif /* end of include guard RESPONSE_HPP */

