/******************************************************************************
 *             _/((		@author		: abaudot (aimebaudot@gmail.com)		  *
 *    _.---. .'   `\		@created	: Thursday Mar 24, 2022 15:27:10 CET, *
 *  .'      `     ^ T=	@filename	: Response		 \)\_					  *
 * /     \       .--'						        /    '. .---._            *
 *|      /       )'-.			                  =P ^     `      '.          *
 *; ,   __..-(   '-.)				               `--.       /     \         *
 * \ \-.__)    ``--._)				               .-'(       \      |        *
 *  '.'-.__.-.						              (.-'   )-..__>   , ;        *
 *    '-...-'                                      (_.--``    (__.-/ /        *
 *******************************************************************************/

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Cgi.hpp"
# include "ServerBlock.hpp"


# include <dirent.h> /* opendir */
# include <sys/stat.h> /* stat function */

class Response
{
	typedef std::map<std::string, std::string>		Headers;
	typedef std::map<int, std::string>				StatusMap;
	typedef std::map<std::string, std::string>		TypeMap;;

	private: /* static data */
	static StatusMap _initStatusMap();
	static StatusMap __statusCode;
	static TypeMap	_initTypeMap();
	static TypeMap	__MIMEtypes;

	/* private data */
	Headers		_headers;

	ServerBlock	*_server;
	Request 	*_request;

	std::string _body;
	std::string _page; //payload

	int			_status;

	public:
	/* constructor */
	Response ( ServerBlock *serv, Request *request ):
		_server(serv),
		_request(request),
		_status(request->get_code()){}

	explicit Response ( int const code ):
		_server(0),
		_request(0),
		_status(code){}

	/* destructor */
	~Response (){}

	void	build () {
		if (_request && _status < BAD_REQUEST)
			_construct_body();
		if (_status >= BAD_REQUEST) {
			if (_request) {
				std::string path = _server->get_error_page(_status,
						_request->get_host());
				if (path != "")
					_body = _get_file_content(path, false);
				if (_body == "")
					_body = _generate_status_page(_status);
			} else {
				_body = _generate_status_page(_status);
			}
		}
		_page  = _construct_header() + _body + "\r\n";
	}

	const char *get_response() const { return _page.c_str(); }

	size_t	size() const { return _page.size(); }


	private: /* private functions */

	void	_construct_body () {
		const LocationBlock  *lblock = _server->get_lockBlock(
				_request->get_host(), _request->get_uri());
		if (lblock->get_body_limit() < _request->get_raw().size()) {
			_status = PAYLOAD_TOO_LARGE;
			return ;
		}

		switch (_request->get_method()) {
			case _GET:
				_get(lblock); break;
			case _POST:
				_post(lblock); break;
			case _DELETE:
				_delete(lblock); break;
			default:
				_status = METHOD_NOT_ALLOWED;
		}
	}

	/* --------- Header Generation ------- */
	static std::string _toString( size_t size ) { /* only used in _construct_header */
		std::stringstream ss;
		ss << size;
		return (ss.str());
	}
	void	_set_header_date() { /* only used int _construct_header */
		std::time_t t = std::time(NULL);
		static char buf[30];

		strftime(buf, sizeof(buf), "%a, %d %b %Y %T %Z", std::localtime(&t));
		_headers["Date"] = std::string(buf);
	}
	std::string _construct_header () {
		if (!_request || (_request &&_request->closed()))
			_headers["Connection"] = "closed";
		else
			_headers["Connection"] = "keep-alive";

		if (_headers["Content-Type"] == "") {
			if (_status != OK)
				_headers["Content-Type"] = __MIMEtypes[".html"];
			else if (_request)
				_headers["Content-Type"] = __MIMEtypes[_request->get_uri()];
			else
				_headers["Content-Type"] = __MIMEtypes["/"];
		}
		_headers["Content-Length"] = _toString(_body.size());
		_set_header_date();
		_headers["Server"] = "Webserb/v0.1";

		std::stringstream head;
		head << "HTTP/1.1 " << _status << " " << __statusCode[_status] << "\r\n";
		std::string headers;
		Headers::const_iterator hit = _headers.begin();
		for (; hit != _headers.end(); ++hit)
			headers += hit->first + ": " + hit->second + "\r\n";
		return (head.str() + headers + "\r\n");
	}
	
	/* Commun for  all method: build the requested path */
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

	/* ----------------- GET Method --------------------- */
	void	_get( LocationBlock const *lblock ) {
		std::string const path = _build_method_path(lblock, _GET, false);

		if (path == "")
			return ;

		if (lblock->get_redirection() != "")
			return ((void)_do_redirection(lblock)); //
		_get_file(lblock, path);
	}

	bool	_get_file( LocationBlock const *lblock,
			std::string const &path ) {
		/* try cgi */
		if (_cgi_pass(lblock))
			return (true);

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
			return (_get_dir(lblock, _request->get_uri()));
		_body = _get_file_content(path, true);
		return (true);
	}

	bool	_get_dir( LocationBlock const *lblock, std::string const &path ) {

		std::vector<struct dirent> files;
		/* 1) get all the files in paht */
		if (!_dump_files(path, files))
			return (false);
		/* 2) find the proper file whit reditection*/
		if (_get_index(lblock, path, files))
			return (true);
		/* 3) simple way: just a listing*/
		if (lblock->get_autoindex() == true)
			return (_get_autoindex(files));
		_status = NOT_FOUND;
		return (false); /* usage ? */
	}
	bool _dump_files( std::string const &path,
			std::vector<struct dirent> &dir_bucket ) {

		errno = 0;
		/* open the directory pointed by the path */
		DIR	*dirptr = opendir(path.c_str());
		if (!dirptr) { /* error set status */
			if (errno == ENOENT)
				_status = NOT_FOUND;
			else if (errno == EACCES || errno == EPERM)
				_status = FORBIDDEN;
			else
				_status = INTERNAL_SERVER_ERROR;
			return (false);
		}
		/* get all directory in there */
		struct dirent *file;
		while ((file = readdir(dirptr))) {
			if (errno) {
				_status = INSUFFICIENT_STORAGE;
				closedir(dirptr);
				return (false);
			}
			dir_bucket.push_back(*file);
		}
		closedir(dirptr);
		return (true);
	}
	bool	_get_index( LocationBlock const *lblock, std::string const &path,
			std::vector<struct dirent> &files) {
		LocationBlock::IndexObject const &indexs = lblock->get_indexs();
		if (indexs.size() <= 0)
			return (false);

		LocationBlock::IndexObject::const_iterator it;
		for (it = indexs.begin(); it != indexs.end(); it++) {
			std::vector<struct dirent>::const_iterator	it_file;
			for (it_file = files.begin(); it_file != files.end(); it_file++)
				if (it_file->d_name == *it)
					return _get_file(lblock, path + "/" + it_file->d_name);
		}
		return (false);
	}
	bool	_get_autoindex( std::vector<struct dirent> const &files ) {
		std::vector<struct dirent>::const_iterator it_file = files.begin();
		/* maybe do something better (order etc) */
		for (; it_file != files.end(); it_file++)
 		{
 			_body += "<html>\n<head><title>Autoindex</title></head>\n";
 			_body += "<a href=\"" + std::string(it_file->d_name) + "\">" + std::string(it_file->d_name) + "</a>" + "<br>\n";
 			_body += "\n</html>\n";
 		}
		return (true);
	}

	std::string _get_file_content( std::string const &path, bool stat ) {
		int fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
		if (fd == -1) {
			if (stat) {
				if (errno == EACCES)
					_status = FORBIDDEN;
				_status = INTERNAL_SERVER_ERROR;
			}
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

	void _do_redirection( LocationBlock const *lblock ) {
		_status = lblock->get_redirection_code();
		_headers["Location"] = lblock->get_redirection();
	}

	/* ----------- POST Method ------------ */

	/* POST /upload?upload_progress_id=12344 HTTP/1.1
		Host: localhost:3000
		Content-Length: 1325
		Origin: http://localhost:3000
		... other headers ...
		Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryePkpFF7tjBAqx29L
		------WebKitFormBoundaryePkpFF7tjBAqx29L
		Content-Disposition: form-data; name="MAX_FILE_SIZE"

			100000
		------WebKitFormBoundaryePkpFF7tjBAqx29L
		Content-Disposition: form-data; name="uploadedfile"; filename="hello.o"
		Content-Type: application/x-object

		... contents of file goes here ...
		------WebKitFormBoundaryePkpFF7tjBAqx29L--
	*/
	void	_post( LocationBlock const *lblock ) {
		const std::string path = _build_method_path(lblock, _POST, true);

		if (path == "")
			return ;

		/* try cgi */
		if (_cgi_pass(lblock))
			return ;

		_handle_upload(lblock, path);
	}
	void
		_handle_upload( LocationBlock const *block, std::string const &path ) {

			if (block->get_upload_pass() == "") {
				_status = FORBIDDEN;
				return ;
			}
			std::string const
				content_type = _request->get_header_field("content-type");
			if (content_type == "") {
				_create_file(path, _request->get_raw());
				return ;
			}
			if (_request->get_form() == _MULTIPART)
				return (_multipart_upload(path,
							content_type.substr(content_type.find("=") + 1)));
			_status = METHOD_NOT_ALLOWED;
			/* I've chose to only handle multipart */
		}

	void
		_multipart_upload( std::string const &path, std::string const &boundary ) {

		std::string body = _request->get_raw();
		size_t	line = body.find("\r\n"); // bondary line
		size_t start = line + 2;
		line = body.find("\r\n", start);

		while (line != std::string::npos) {

			std::string const content_disposition = body.substr(start, line);
			start = line + 2;
			start = body.find("\r\n", start) + 4; // remove content_type

			/* get filename */
			size_t const
				filename_pos = content_disposition.find("filename=\"", 0) + 10;
			std::string const
				filename = content_disposition.substr(filename_pos,
					content_disposition.find("\"", filename_pos) - filename_pos);

			line = body.find(boundary, start);
			/* last file */
			if (body[line + boundary.size() - 1] == '-') {
				_create_file(path + filename, body.substr(start,
							line - start - 4));
				return ;
			}
			/* one more file */
			if (!_create_file(path + filename, body.substr(start,
							line - start - 2)))
				return ;
			start = line + boundary.size() + 4;
			line = body.find("\r\n", start);
		}
		_status = NO_CONTENT;
	}
	bool
		_create_file( std::string const &path, std::string const &content ) {

		/* not sure about this one. File should not already exist */
		if (access(path.c_str(), F_OK) == 0) {
			_status = CONFLICT;
			return (false);
		}

		std::cout << " [🤰] Creating: " << path << "\n";
		std::ofstream _out(path.c_str());
		if (!_out) {
			_status = INTERNAL_SERVER_ERROR;
			return (false);
		}
  		_out << content;
		if (_out.bad()) {
			_status = INTERNAL_SERVER_ERROR;
			return (false);
		}
		_out.close();
		_status = NO_CONTENT;
		usleep(10); //?
		return (true);
	}

	/* --------- CGI ----------*/

	bool	_cgi_pass( LocationBlock const *lblock ) {
		std::string cgi_path = lblock->get_cgi(_request->get_uri());

		if (cgi_path == "")
			return (false);

		/* init cgi */
		Cgi cgi(cgi_path, lblock->get_root() + _request->get_uri(),
				_request->get_query(), _request->get_method());

		/* setup  cgi */
		if (!cgi.setup(_request->get_raw(), _request->get_headers())) {
			_status = INTERNAL_SERVER_ERROR;
			return (true);
		}

		/* run cgi script */
		if (!cgi.run()) {
			if (cgi.timed_out())
				_status = GATEWAY_TIMEOUT;
			else
				_status = INTERNAL_SERVER_ERROR;
			return (true);
		}

		/* retrive cgi output */
		_body = cgi.get_output();

		/* add the cgi header to the response */

		for (Cgi::HeadersObject::const_iterator it = cgi.Hbegin(); it != cgi.Hend(); ++it)
			_headers[it->first] = it->second;
		return (true);
	}

	/* --------- DELET ------- */
	void	_delete( LocationBlock const *block ) {

		const std::string path = _build_method_path(block, _DELETE, true);
		if (path == "")
			return;
		errno = 0;

		std::cout << " [👾] Deleting: " << path << "\n";
		if (remove(path.c_str()) != -1) {
			_status = NO_CONTENT;
			return ;
		}

		if (errno == ENOENT || errno == ENOTDIR)
			_status = NOT_FOUND;
		else if (errno == EACCES || errno == EPERM || errno == 39)
			_status = FORBIDDEN;
		else
			_status = INTERNAL_SERVER_ERROR;
	}
	/* ------------------------------ */

	const std::string	_generate_status_page( int const status_code ) {
		std::stringstream	ss;
		ss << status_code;

		return "<!DOCTYPE html>\n"
			"<html>\n"
			"	<head>\n"
			"		<style>\n"
			"			html { color-scheme: light dark; }\n"
			"			body { width: 35em; margin: 0 auto;\n"
			"			font-family: monospace, Tahoma, Verdana, Arial, sans-serif; }\n"
			"		</style>\n"
			"		<title>" + ss.str() + " - " + _resolve_code(status_code) + "</title>\n"
			"	</head>\n"
			"	<body>\n"
			"		<h1>" + ss.str() + " - " + _resolve_code(status_code) + "</h1>\n"
			"		<hr />\n"
			"		<p><em>Webserv</em></p>\n"
			"	</body>\n"
			"</html>";
	}

	std::string	const _resolve_code( int status_code ) {
		StatusMap::iterator it = __statusCode.find(status_code);
		if (it == __statusCode.end())
			return (__statusCode[0]);
		return (it->second);
	}
};
#endif /* end of include guard RESPONSE_HPP */
