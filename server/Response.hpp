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
	/* static data */ //should they be public ?
	static std::map<int, std::string> __statusCode;
	static std::map<std::string, std::string> __MIMEtypes;

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
	void	_construct_body () {
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
		/* ------------------------   later 
		else if (meth == _POST)
			_post(lblock);
		else if (meth == _DELETE)
			_delete(lblock);
		*/
		else
			_status = METHOD_NOT_ALLOWED;
	}

	std::string _construct_header () {
		if (!_request || (_request &&_request->closed()))
			_headers["Connection"] = "closed";
		else
			_headers["Connection"] = "keep-alive";

		if (_headers["Content-Type"] == "") {
			if (_status != _OK)
				_headers["Content-Type"] = __MIMEtypes[".html"];
			else if (_request)
				_headers["Content-Type"] = __MIMEtypes[_req->get_uri()];
			else
				_headers["Content-Type"] = __MIMEtypes["/"];
		}
		_headers["Content-Length"] = _toString(_body.size());
		_set_header_date();
		_headers["Server"] = "Webserb/v0.1";

		std::stringstream head;
		head << "HTTP/1.1 " << _status << " " << __statusCode(_status) << "\r\n";
		std::string headers;
		Headers::const_iterator hit = _headers.begin();
		for (; hit != _headers.end(); ++hit)
			headers += hit->first + ": " + hit->second + "\r\n";
		return (head.str() + headers + "\r\n");
	}

	std::string _toString(size_t size) {
		std::stringstream ss;
		ss << size;
		return (ss.str());
	}

	void	_set_header_date() {
		std::time_t t = std::time(NULL);
		static char buf[30];

		strftime(buf, sizeof(buf), "%a, %d %b %Y %T %Z", std::localtime(&t));
		_headers["Date"] = std::string(buf);
	}


	/* ----------------- GET ------------------------- */
	void	_get( LocationBlock const *lblock ) {
		std::string const path = _build_method_path(lblock, _GET, false);
		if (path == "")
			return ;

		if (block->get_redirection() != "")
			return ((void)_do_redirection(lblock)); //
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
		 * if cig != ""
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
		/* 1) get all the files in paht */
		if (!_dump_files(path, files))
			return (false);
		/* 2) find the proper file whit reditection*/
		if (_get_index(lblock, path, files))
			return (true);
		/* 3) */
		/* not yet ----------------------------- Alina
		if (lblock->is_autoindex() == true)
			return (_get_autoindex(files, path));
		*/
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
				_status = NOT_FOUND
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
			bucket.push_back(*file);
		}
		closedir(dirptr);
		return (true);
	}
	bool	_get_index( LocationBlock const *lblock, std::string const &path,
		std::vector<struct dirent> &files) {
		LocationBlock::IndexObject indexs = lblock->get_indexs();
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

	void _do_redirection( LocationBlock const *lblock ) {
		_status = lblock->get_redirection_code();
		_headers["Location"] = block->get_redirection();
	}
};
// ------------------------------------------------------------------
void	init_MIME_types_map( std::map<std::string, std::string> &MIMEtypes ) {
	MIME_TYPES["aac"] = "audio/aac";
	MIME_TYPES[".abw"] = "application/x-abiword";
	MIME_TYPES[".arc"] = "application/x-freearc";
	MIME_TYPES[".avi"] = "video/x-msvideo";
	MIME_TYPES[".azw"] = "application/vnd.amazon.ebook";
	MIME_TYPES[".bin"] = "application/octet-stream";
	MIME_TYPES[".bmp"] = "image/bmp";
	MIME_TYPES[".bz"] = "application/x-bzip";
	MIME_TYPES[".bz2"] = "application/x-bzip2";
	MIME_TYPES[".cda"] = "application/x-cdf";
	MIME_TYPES[".csh"] = "application/x-csh";
	MIME_TYPES[".css"] = "text/css";
	MIME_TYPES[".csv"] = "text/csv";
	MIME_TYPES[".doc"] = "application/msword";
	MIME_TYPES[".docx"] =
		"application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	MIME_TYPES[".eot"] = "application/vnd.ms-fontobject";
	MIME_TYPES[".epub"] = "application/epub+zip";
	MIME_TYPES[".gz"] = "application/gzip";
	MIME_TYPES[".gif"] = "image/gif";
	MIME_TYPES[".htm"] = "text/html";
	MIME_TYPES[".html"] = "text/html";
	MIME_TYPES[".ico"] = "image/vnd.microsoft.icon";
	MIME_TYPES[".ics"] = "text/calendar";
	MIME_TYPES[".jar"] = "application/java-archive";
	MIME_TYPES[".jpeg"] = "image/jpeg";
	MIME_TYPES[".jpg"] = "image/jpeg";
	MIME_TYPES[".js"] = "text/javascript";
	MIME_TYPES[".json"] = "application/json";
	MIME_TYPES[".jsonld"] = "application/ld+json";
	MIME_TYPES[".mid"] = "audio/midi";
	MIME_TYPES[".midi"] = "audio/x-midi";
	MIME_TYPES[".mjs"] = "text/javascript";
	MIME_TYPES[".mp3"] = "audio/mpeg";
	MIME_TYPES[".mp4"] = "video/mp4";
	MIME_TYPES[".mpeg"] = "video/mpeg";
	MIME_TYPES[".mpkg"] = "application/vnd.apple.installer+xml";
	MIME_TYPES[".odp"] = "application/vnd.oasis.opendocument.presentation";
	MIME_TYPES[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	MIME_TYPES[".odt"] = "application/vnd.oasis.opendocument.text";
	MIME_TYPES[".oga"] = "audio/ogg";
	MIME_TYPES[".ogv"] = "video/ogg";
	MIME_TYPES[".ogx"] = "application/ogg";
	MIME_TYPES[".opus"] = "audio/opus";
	MIME_TYPES[".otf"] = "font/otf";
	MIME_TYPES[".png"] = "image/png";
	MIME_TYPES[".pdf"] = "application/pdf";
	MIME_TYPES[".php"] = "application/x-httpd-php";
	MIME_TYPES[".ppt"] = "application/vnd.ms-powerpoint";
	MIME_TYPES[".pptx"] =
		"application/vnd.openxmlformats-officedocument.presentationml.presentation";
	MIME_TYPES[".rar"] = "application/x-rar-compressed";
	MIME_TYPES[".rtf"] = "application/rtf";
	MIME_TYPES[".sh"] = "application/x-sh";
	MIME_TYPES[".svg"] = "image/svg+xml";
	MIME_TYPES[".swf"] = "application/x-shockwave-flash";
	MIME_TYPES[".tar"] = "application/x-tar";
	MIME_TYPES[".tif"] = "image/tiff";
	MIME_TYPES[".tiff"] = "image/tiff";
	MIME_TYPES[".ttf"] = "font/ttf";
	MIME_TYPES[".ts"] = "video/mp2t";
	MIME_TYPES[".txt"] = "text/plain";
	MIME_TYPES[".vsd"] = "application/vnd.visio";
	MIME_TYPES[".wav"] = "audio/wav";
	MIME_TYPES[".weba"] = "audio/webm";
	MIME_TYPES[".webm"] = "video/webm";
	MIME_TYPES[".webp"] = "image/webp";
	MIME_TYPES[".woff"] = "font/woff";
	MIME_TYPES[".woff2"] = "font/woff2";
	MIME_TYPES[".xhtml"] = "application/xhtml+xml";
	MIME_TYPES[".xls"] = "application/vnd.ms-excel";
	MIME_TYPES[".xlsx"] =
		"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	MIME_TYPES[".xml"] = "application/xml";
	MIME_TYPES[".xul"] = "application/vnd.mozilla.xul+xml";
	MIME_TYPES[".zip"] = "application/zip";
	MIME_TYPES[".3gp"] = "video/3gpp";
	MIME_TYPES[".3g2"] = "video/3gpp2";
	MIME_TYPES[".7z"] = "application/x-7z-compressed";
}

void	init_status_map( std::map<int, std::string> &statusCode ) {
	statusCode[CONTINUE] = "Continue";
	statusCode[SWITCHING_PROTOCOLS] = "Switching Protocols";
	statusCode[PROCESSING] = "Processing";
	statusCode[EARLY_HINTS] = "Early Hints";
	statusCode[OK] = "OK";
	statusCode[CREATED] = "Created";
	statusCode[ACCEPTED] = "Accepted";
	statusCode[NON_AUTHORITATIVE_INFORMATION] = "Non-Authoritative Information";
	statusCode[NO_CONTENT] = "No Content";
	statusCode[RESET_CONTENT] = "Reset Content";
	statusCode[PARTIAL_CONTENT] = "Partial Content";
	statusCode[MULTI_STATUS] = "Multi-Status";
	statusCode[ALREADY_REPORTED] = "Already Reported";
	statusCode[IM_USED] = "IM Used";
	statusCode[MULTIPLE_CHOICES] = "Multiple Choices";
	statusCode[MOVED_PERMANENTLY] = "Moved Permanently";
	statusCode[FOUND] = "Found";
	statusCode[SEE_OTHER] = "See Other";
	statusCode[NOT_MODIFIED] = "Not Modified";
	statusCode[USE_PROXY] = "Use Proxy";
	statusCode[SWITCH_PROXY] = "Switch Proxy";
	statusCode[TEMPORARY_REDIRECT] = "Temporary Redirect";
	statusCode[PERMANENT_REDIRECT] = "Permanent Redirect";
	statusCode[BAD_REQUEST] = "Bad Request";
	statusCode[UNAUTHORIZED] = "Unauthorized";
	statusCode[PAYMENT_REQUIRED] = "Payment Required";
	statusCode[FORBIDDEN] = "Forbidden";
	statusCode[NOT_FOUND] = "Not Found";
	statusCode[METHOD_NOT_ALLOWED] = "Method Not Allowed";
	statusCode[NOT_ACCEPTABLE] = "Not Acceptable";
	statusCode[PROXY_AUTHENTICATION_REQUIRED] = "Proxy Authentication Required";
	statusCode[REQUEST_TIMEOUT] = "Request Timeout";
	statusCode[CONFLICT] = "Conflict";
	statusCode[GONE] = "Gone";
	statusCode[LENGTH_REQUIRED] = "Length Required";
	statusCode[PRECONDITION_FAILED] = "Precondition Failed";
	statusCode[PAYLOAD_TOO_LARGE] = "Payload Too Large";
	statusCode[REQUEST_URI_TOO_LONG] = "Request URI Too Long";
	statusCode[UNSUPPORTED_MEDIA_TYPE] = "Unsupported Media Type";
	statusCode[REQUESTED_RANGE_NOT_SATISFIABLE] = "Requested Range Not Satisfiable";
	statusCode[EXPECTATION_FAILED] = "Expectation Failed";
	statusCode[IM_A_TEAPOT] = "I'm a teapot";
	statusCode[UNPROCESSABLE_ENTITY] = "Unprocessable Entity";
	statusCode[LOCKED] = "Locked";
	statusCode[FAILED_DEPENDENCY] = "Failed Dependency";
	statusCode[UPGRADE_REQUIRED] = "Upgrade Required";
	statusCode[PRECONDITION_REQUIRED] = "Precondition Required";
	statusCode[TOO_MANY_REQUESTS] = "Too Many Requests";
	statusCode[REQUEST_HEADER_FIELDS_TOO_LARGE] = "Request Header Fields Too Large";
	statusCode[UNAVAILABLE_FOR_LEGAL_REASONS] = "Unavailable For Legal Reasons";
	statusCode[INTERNAL_SERVER_ERROR] = "Internal Server Error";
	statusCode[NOT_IMPLEMENTED] = "Not Implemented";
	statusCode[BAD_GATEWAY] = "Bad Gateway";
	statusCode[SERVICE_UNAVAILABLE] = "Service Unavailable";
	statusCode[GATEWAY_TIMEOUT] = "Gateway Timeout";
	statusCode[HTTP_VERSION_NOT_SUPPORTED] = "HTTP Version Not Supported";
	statusCode[VARIANT_ALSO_NEGOTIATES] = "Variant Also Negotiates";
	statusCode[INSUFFICIENT_STORAGE] = "Insufficient Storage";
	statusCode[LOOP_DETECTED] = "Loop Detected";
	statusCode[NOT_EXTENDED] = "Not Extended";
	statusCode[NETWORK_AUTHENTICATION_REQUIRED] = "Network Authentication Required";
}
#endif /* end of include guard RESPONSE_HPP */

