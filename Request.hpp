/**
 * @author      : abaudot (aimebaudot@gmail.com)
 * @file        : Request
 * @created     : Thursday Mar 17, 2022 15:33:30 CET
 */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "enum.hpp"

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
		std::string	_raw;
		std::string	_host;
		std::string	_uri; /* Uniform Resource Identifier */
		std::string	_version;
		std::string	_query;
		METHODS		_method;
		size_t		_separator_pos;
		int			_code
		uint8_t		_status;

	public:
		/* constructor */
		explicit Request ( std::string const &buffer ):
		_headers(),
		_raw(buffer),
		_host(),
		_uri(),
		_version(),
		_query()
		_method(_UNKNOWN),
		_status(0){}

		/* destructor */
		~Request () {}

		/* getter */
		METHODS	get_method() const { return _method; }
		std::string const &get_uri() { return _uri; }
		std::string const &get_query() { return _query; }
		std::string const &get_host() { return _host; }

		/* methods */
		void	add_buffer( std::string const &buffer ) {
			_raw += buffer;
		}

		bool	init() {
			if (!_extract_method())
				return (false);
			if (!_extract_uri())
				return (false);
			if (!_extract_version())
				return (false);
			if (!_extract_headers(&_headers))
				return (false);
			//
			//
			_status |= HEADER_READY;
			return (true);
		}

	private:
		/* private data */
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
		return (_UNKNOWN);
	}

	bool	_extract_method() {
		_separator_pos = _raw.find(" ");
		if (separator_pos == std::string::npos) //no separator found
			return _invalid_(BAD_REQUEST);

		_method = _get_method(_raw_request.substr(0, separator_pos));

		/*uptade _raw */
//		_raw_request.erase(0, separator_pos + 1); //could be done faster with handwriten buffer
		++_separator_pos;

		if (_method == METH_UNKNOWN) // could not get the method right
			return _invalid(NOT_IMPLEMENTED); // change this
		return (true);
	}

	bool	_extract_uri() {
		size_t const tmp = _raw.find(" ", _separator_pos);
		/* get uri */
		if (tmp == std::string::npos) //no separator
			return (_invalid(BAD_REQUEST));
		/*
		if (tmp > 8192)
			return (_invalid(REQUEST_URI_TOO_LONG));
		*/
		_uri = _raw.substr(tmp, _separator_pos);

		if (_uri == "" || _uri.find[0] != 0) // (trully) not certain about the last condition... 
			return (_invalid(BAD_REQUEST));
		/* uptdate _raw */
//		_raw_request.erase(0, separator_pos + 1);
		_separator_pos = tmp + 1;

		/* get the _query if there is one */
		if (_uri.find("?", _separator_pos) != std::string::npos) { //so far I undestand that query start after a '?' 
			_query = _uri.substr(_uri.find("?") + 1);
			//_uri.erase(_uri.find("?"));
		}
		return (true);
	}

	static std::string *_strtolower(std::string *s) {
		for (std::string::iterator it = s->begin(); it != s->end(); it++)
			*it = std::tolower(*it);
		return s;
	}

	bool	_extract_version() {
		/* get version */
		size_t const tmp = _raw.find("\r\n", _separator_pos);
		if (tmp == std::string::npos)
			return (_invalid(BAD_REQUEST));
		_version = _raw.substr(tmp, _separator_pos);
		_strtolower(&_version);
		if (_version == "" || _version.find("http", tmp) != 0
			|| _version.find[4] != '/') == std::string::npos)
			return _invalid_request(BAD_REQUEST);
		_http_version = _http_version.substr(_version + 5);
		if (_http_version != "1.1")
			return _invalid_request(HTTP_VERSION_NOT_SUPPORTED);
//		_raw_request.erase(0, version_separator_pos + 2);
		_separator_pos = tmp + 2;
		return (true);
	}

	static std::string* _trim(std::string *s, const char *t = " \t") {
	    s->erase(s->find_last_not_of(t) + 1);
	    s->erase(0, s->find_first_not_of(t));
		return (s);
	}

	bool	_extract_headers(Headers *bucket) {
		size_t header_size = 0;
		size_t	tmp  = _raw.find("\r\n", _separator_pos);
		while (tmp != std::string::npos) {
			std::string	header_str = _raw.substr(_separator_pos, tmp);
			size_t		name_separator_pos = header_str.find(":");
			if (name_separator_pos == std::string::npos)
				break;
			std::string	header_name = header_str.substr(0, name_separator_pos);
			std::string	header_value = header_str.substr(name_separator_pos + 1);
			_strtolower(&header_name);
			//if (header_name != "cookie")
			_strtolower(&header_value);
			_trim(&header_value);
			(*bucket)[header_name] = header_value;
			if (header_name == "host")
				_host = header_value;
			_separator_pos = tmp + 2;
			_raw_request.erase(0, _separator_pos);
			header_size += _separator_pos;
			if (header_size > 16384)
				return (_invalid_request(REQUEST_HEADER_FIELDS_TOO_LARGE));
			header_separator_pos = _raw_request.find("\r\n");
		}
		return true;
	}

	/*
	bool	_validate_host() {
		HeadersObject::const_iterator it = _headers.find("host");
		if (it == _headers.end() || it->second == "") {
			return _invalid_request(BAD_REQUEST);
		}
		return true;
	}

	bool	_validate_post() {
		HeadersObject::const_iterator it = _headers.find("transfer-encoding");
		if (it != _headers.end() &&
			it->second.find("chunked") != std::string::npos) {
			_chunked = true;
		} else {
			it = _headers.find("content-length");
			if (it == _headers.end() || it->second == "")
				return _invalid_request(BAD_REQUEST);
			_body_size = static_cast<size_t>(strtol(it->second.c_str(), NULL, 10));
		}
		it = _headers.find("content-type");
		if (it == _headers.end() || it->second == "")
			return _invalid_request(BAD_REQUEST);
		if (it->second == "application/x-www-form-urlencoded")
			_post_form = _URLENCODED;
		else if (it->second == "multipart/form-data")
			_post_form = _MULTIPART;
		return true;
	}
	*/

	bool	_invalid_request(STATUS_CODE http_code) {
		_code = http_code;
		_status |= CLOSED;
		return (false);
	}

};

#endif /* end of include guard REQUEST_HPP */

