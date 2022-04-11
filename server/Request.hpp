/**
 * @author      : abaudot (aimebaudot@gmail.com)
 * @file        : Request
 * @created     : Thursday Mar 17, 2022 15:33:30 CET
 */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "enum.hpp"
# include <unordered_map>
# include <iostream> /*debug*/
# include <string>
# include <sys/time.h>

# define HEADER_READY 1
# define BODY_READY 2
# define CHUNKED 4
# define CLOSED 8

/*
 * Request = Request_Line            ;
 * 			*((general_header        ;
 * 			  | request_header       ;
 * 			  | entity_header) CRLF) ;
 * 			 [ message-body ]        ;
 */

/* Request-Line   = Method SP Request-URI SP HTTP-Version CRLF */
/* Request-URI    = "*" | absoluteURI | abs_path | authority */

/*
 * uri explemple
 **
 foo://username:password@example.com:8042/over/there/index.dtb?type=animal;name=ferret#nose
  \ /   \________________/\_________/ \__/            \___/ \_/ \_____________________/ \__/
   |           |               |       |                |    |           |                |
   |          user            host    port              |    |          query         fragment
   |    \_______________________________/ \_____________|____|/
 sheme                  |                         |     |    |
   |                 domaine                     path   |    |
   |                                                    |    |
   |            path                         can be interpreted as a file name 
   |   ___________|____________                              |
  / \ /                        \                             |
  urn:example:animal:ferrett:nose [archive]         can be interpreted as an extension
**
*/


class Request
{
	public:
		typedef std::unordered_map<std::string, std::string> Headers;
		/**
		 * methods Token: indicate the methode to perform on the resource 
		 * indentified by the Request-URI
		 */
	private:
		/* private data */
		Headers		_headers;
		std::string	_raw; /* request data to parse */
		std::string	_host;
		std::string	_uri; /* Uniform Resource Identifier */
		std::string	_query;
		size_t		_separator_pos; /* only use for init.... move it there */
		size_t		_body_size; /*not sur I need it */
		STATUS_CODE	_code; /* use for response */
		METHODS		_method; /* GET POST DELET */
		FORM		_form; /* use for response */
		uint8_t		_status; /* bits info */

	public:
		/* constructor */
		explicit Request ( char const *buffer, int const n ):
		_headers(),
		_raw(buffer, buffer + n),
		_host(),
		_uri(),
//		_version(),
		_query(),
		_body_size(0),
		_code(OK),
		_method(METHOD_UNKNOWN),
		_form(FORM_UNKNOWN),
		_status(0){}

		/* destructor */
		~Request () {}

#ifdef DEBUG
		/* ---------- for debug only ------------- */
		void	print() const {
			std::cout << "-------------------------------------------\n\n";
			std::cout << BLUE << "Host: " << EOCC << _host << "\n";
			std::cout << BLUE << "Method: " << EOCC << _method << "\n";
			std::cout << BLUE << "URI: " << EOCC << _uri << "\n";
			std::cout << BLUE << "version: " << EOCC << "HTTP 1.1" << "\n";
			std::cout << GREEN << "query: " << EOCC << _query<< "\n";
			std::cout << BLUE << "Headers => " << EOCC << "\n-----------------------\n";
			for (Headers::const_iterator cit = _headers.begin(); cit != _headers.end(); ++cit)
				std::cout << PURPLE << (cit)->first << " : " << EOCC << (cit)->second
					<< "\n";
			std::cout << "\n -----------------------\n";
			std::cout << BLUE << "body size: " << EOCC << _body_size << " (should be 0 except the method is POST)\n";
			if (_body_size)
				std::cout << BLUE << "Body: " << EOCC << _raw << "\n";
			std::cout << RED << "status code: " << EOCC << _code << "\n";
			std::cout <<  "hidden status: " << (int)_status << "\n";
			std::cout << "-------------------------------------------\n\n";
		}
#endif

		/* getter */
		Headers const &get_headers() const { return _headers; }
		std::string const &get_raw() const { return _raw; }
		std::string const &get_uri() const { return _uri; }
		std::string const &get_query() const { return _query; }
		std::string const &get_host() const { return _host; }
		std::string const &get_header_field(std::string const &key) {
			return (_headers[key]);
		}
		STATUS_CODE	get_code() const { return _code; }
		bool	header_ready() const { return ((_status & HEADER_READY)); }
//		bool	body_ready() const { return ((_status & BODY_READY)); }
		bool	have_read_enought() const { return (_raw.find("\r\n\r\n") != std::string::npos); }
		METHODS	get_method() const { return _method; }
		FORM	get_form() const { return _form; };

		/* methods */
		void	add_buffer( char const *buffer, int const size ) {
			_raw.insert(_raw.size(), buffer, size);
		}

		bool	read_header() {
#ifdef DEBUG
			std::cout << _raw << "\n";
#endif
			if (!_extract_method())
				return (false);
			if (!_extract_uri())
				return (false);
			if (!_check_version())
				return (false);
			if (!_extract_headers())
				return (false);
			if (!_validate_host())
				return (false);
			if (_method == _POST && _validate_post() == false)
				return (false);
			_raw.erase(0, 2); //sould erase all the header
			_status |= HEADER_READY;
			return (true);
		}

	bool	read_body() {
		if (_status & CHUNKED) {
			if (_read_chunks() == READ_WAIT)
				return (false);
			_body_size = _raw.size();
			_status &= ~CHUNKED;
//			_status |= BODY_READY; // ? not used
			return (true);
		}
		std::cout << " [📚] readed: " << GREEN
			<< _raw.size() << EOCC << " / " << BLUE << _body_size << EOC;
		if (_raw.size() < _body_size) // not enought
			return (false);
//		_status |= BODY_READY; //not used
		return (true);
	}

	bool	closed() {
		if (!(_status & CLOSED)) {
			Headers::const_iterator cit = _headers.find("connection");
			if (cit != _headers.end()) {
				if (cit->second == "close") {
					_status |= CLOSED;
					return (true);
				}
			}
		}
		return ((_status & CLOSED) != 0);
	}

	private:
	/* private methods */
	/** basic request exemple:
	 * GET / HTTP/1.1
	 * Host: localhost:8000
	 * Connection: keep-alive
	 * sec-ch-ua: " Not A;Brand";v="99", "Chromium";v="98", "Google Chrome";v="98"
	 * sec-ch-ua-mobile: ?0
	 * sec-ch-ua-platform: "macOS"
	 * Upgrade-Insecure-Requests: 1
	 * User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6)
	 * AppleWebKit/537.36 (KHTML, like Gecko) Chrome/98.0.475880 Safari/537.36
	 * Accept:
	 * text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,
	 * image/webp,image/apng,;q=0.8,application/signed-exchange;v=b3;q=0.9
	 * Sec-Fetch-Site: none
	 * Sec-Fetch-Mode: navigate
	 * Sec-Fetch-User: ?1
	 * Sec-Fetch-Dest: document
	 * Accept-Encoding: gzip, deflate, br
	 * Accept-Language: en-US,en;q=0.9
	 **/

	static METHODS _get_method( std::string const &m ) {
		if (m == "GET")
			return (_GET);
		if (m == "POST")
			return (_POST);
		if (m == "DELETE")
			return (_DELETE);
		/* --------- NON IMPLEMENTED --------
		   if (m == "CONNECT")
		   return (_CONNECT);
		   if (m == "OPTIONS")
		   return (_OPTIONS);
		   if (m == "TRACE")
		   return (_TRACE);
		*/
		return (METHOD_UNKNOWN);
	}

	/* get method */
	/* GET */
	bool	_extract_method() {
		_separator_pos = _raw.find(" ");
		if (_separator_pos == std::string::npos) //no separator found
			return _invalid(BAD_REQUEST);

		_method = _get_method(_raw.substr(0, _separator_pos));
		++_separator_pos;

		if (_method == METHOD_UNKNOWN) // could not get the method right
			return _invalid(NOT_IMPLEMENTED); // change this
		return (true);
	}

	/* get uri */
	/* / */
	bool	_extract_uri() {
		size_t const tmp = _raw.find(" ", _separator_pos);
		if (tmp == std::string::npos) //no separator
			return (_invalid(BAD_REQUEST));
		if (tmp > 8192) /* Apach standard */
			return (_invalid(REQUEST_URI_TOO_LONG));
		_uri = _raw.substr(_separator_pos, tmp - _separator_pos);

		if (_uri == "" || _uri[0] != '/') // (trully) not certain about the last condition... 
			return (_invalid(BAD_REQUEST));
		_separator_pos = tmp + 1;

		/* get the _query if there is one */
		if (_uri.find("?", _separator_pos) != std::string::npos) {
			_query = _uri.substr(_uri.find("?") + 1);
		}
		return (true);
	}

	/* check that the version is correct (http/1.1) */
	/* 2.6.  Protocol Versioning */
	/* The version of an HTTP message is indicated by an HTTP-version field
	   in the first line of the message.  HTTP-version is case-sensitive.

	   HTTP-version  = HTTP-name "/" DIGIT "." DIGIT
	*/
	/* HTTP/1.1 */
	bool	_check_version() {
		/* get version */
		std::string	_version;
		size_t const tmp = _raw.find("\r\n", _separator_pos); /* end of the fist line */
		if (tmp == std::string::npos)
			return (_invalid(BAD_REQUEST));
		_version = _raw.substr(_separator_pos, tmp - _separator_pos);
		//_strtolower(&_version); /* need to verify is this is need */
		/* this way is maybe to specific... not sure about the protocol liberty */
		if (_version.find("HTTP/") != 0)
			return (_invalid(BAD_REQUEST));
		if (_version[5] != '1' || _version[6] != '.' || _version[7] != '1')
			return _invalid(HTTP_VERSION_NOT_SUPPORTED);
		_separator_pos = tmp + 2;
		return (true);
	}

	/* usefull ?????? */
	/* YES: 
	 * Each header field consists of a name followed by a colon (":")
	 * and the field value. Field names are case-insensitive.
	 */
	static std::string *_strtolower(std::string *s) {
		for (std::string::iterator it = s->begin(); it != s->end(); it++)
			*it = std::tolower(*it);
		return (s);
	}

	static std::string* _trim(std::string *s, const char *t = " \t") {
		s->erase(s->find_last_not_of(t) + 1);
		s->erase(0, s->find_first_not_of(t));
		return (s);
	}

	/* Connection: keep-alive */
	/* ... */
	/* Accept-Encoding: gzip, deflate, br */
	/* Accept-Language: en-US,en;q=0.9 */
	bool	_extract_headers() {
		size_t header_size = 0;
		size_t	tmp  = _raw.find("\r\n", _separator_pos);

		while (tmp != std::string::npos) {
			std::string	header_str = _raw.substr(_separator_pos, tmp - _separator_pos);
			size_t		name_separator_pos = header_str.find(":");
			if (name_separator_pos == std::string::npos)
				break;
			std::string	header_name = header_str.substr(0, name_separator_pos);
			std::string	header_value = header_str.substr(name_separator_pos + 1);
			_strtolower(&header_name);
			//if (header_name != "cookie")
			//_strtolower(&header_value); //maybe not
			_trim(&header_value);
			_headers[header_name] = header_value;
			if (header_name == "host")
				_host = header_value;
			_separator_pos = tmp + 2;
			header_size += _separator_pos;
			if (header_size > 16384) /* default 16Kb */
				return (_invalid(REQUEST_HEADER_FIELDS_TOO_LARGE));
			 tmp =  _raw.find("\r\n", _separator_pos);
		}
		_raw.erase(0, _separator_pos);
		return (true);
	}

	/* handel chunked transfer mechanism */
	/* at the end of this process _raw == all the body */
	READ	_read_chunks() {
		if (_raw.find("0\r\n\r\n") == std::string::npos) {
			return (READ_WAIT); /*I dont have all body... wainting */
		} else {
			std::string body;
			do {
				const size_t chunk_size_info = _raw.find("\r\n");
				const int64_t chunk_size = strtol(
						_raw.substr(0, chunk_size_info).c_str(), NULL, 16);
				std::cout << "[🔪] Chunking: " << GREEN
					<< "(" << chunk_size << ")" << EOCC << "total: " << BLUE
					<< "(" << body.size() << ")" << EOC;
				if (chunk_size == 0) {
					_raw = body;
					return (READ_OK);
				}
				body += _raw.substr(chunk_size_info + 2, chunk_size);
				_raw.erase(0, chunk_size + chunk_size_info + 4);
			} while (true);
		}
	}

	bool	_validate_host() {
		Headers::const_iterator it = _headers.find("host");
		if (it == _headers.end() || it->second == "") {
			return _invalid(BAD_REQUEST);
		}
		return (true);
	}

	/* for POST method */
	bool	_validate_post() {
		Headers::const_iterator it = _headers.find("transfer-encoding");
		if (it != _headers.end() &&
				it->second.find("chunked") != std::string::npos) {
			_status |= CHUNKED; //chunked mod on
		} else { /* normal mod */
			it = _headers.find("content-length");
			if (it == _headers.end() || it->second == "")
				return (_invalid(BAD_REQUEST));
			_body_size = static_cast<size_t>(strtol(it->second.c_str(), NULL, 10));
		}
		it = _headers.find("content-type");
		if (it == _headers.end() || it->second == "")
			return (_invalid(BAD_REQUEST));
		if (it->second.find("multipart/form-data") != std::string::npos)
			_form = _MULTIPART;
		else  if (it->second.find("application/x-www-form-urlencoded") != std::string::npos)
			_form = _URLENCODED;
		else
			_form = _PLAINTXT;
		return (true);
	}

	bool	_invalid(STATUS_CODE http_code) {
		_code = http_code;
		_status |= CLOSED;
		return (false);
	}
};
#endif /* end of include guard REQUEST_HPP */
