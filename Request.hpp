/**
 * @author      : abaudot (aimebaudot@gmail.com)
 * @file        : Request
 * @created     : Thursday Mar 17, 2022 15:33:30 CET
 */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "enum.hpp"

# include <unnordered_map>

# define IS_INIT 1
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
		std::string	_raw; /* request data to parse */.
		std::string	_host;
		std::string	_uri; /* Uniform Resource Identifier */
//		std::string	_version; /* always http 1.1 should I keep it ? (no) */
		std::string	_query;
		size_t		_separator_pos; /* only use for init.... move it there */
		STATUS_CODE	_code
		METHODS		_method;
		FORM		_form;
		uint8_t		_status; /* bits info */

	public:
		/* constructor */
		explicit Request ( std::string const &buffer ):
		_headers(),
		_raw(buffer),
		_host(),
		_uri(),
//		_version(),
		_query()
		_code(_OK),
		_method(METHOD_UNKNOWN),
		_form(FORM_UNKNOWN),
		_status(0){}

		/* destructor */
		~Request () {}

		/* getter */
		METHODS	get_method() const { return _method; }
		std::string const &get_uri() { return _uri; }
		std::string const &get_query() { return _query; }
		std::string const &get_host() { return _host; }
		bool	get_header_status() { return ((_status & IS_INIT)); } /* init success */

		/* methods */
		void	add_buffer( std::string const &buffer ) {
			_raw += buffer;
		}

		bool	init() {
			if (!_extract_method())
				return (false);
			if (!_extract_uri())
				return (false);
			if (!_check_version())
				return (false);
			if (!_extract_headers(&_headers))
				return (false);
			if (_method == _POST && _validate_post() == false)
				return false;
			//_raw.erase(0, 2);
			_status |= IS_INIT;
			return (true);
		}

	bool	read_body() {
		if (_status & CHUNKED) {
			if (_read_chunks() == READ_WAIT)
				return (false);
			_body_size = _raw_request.size();
			_status &= ˜CHUNKED;
			retun (true);
		}
		if (_raw.size() < _body_size)
			return (false);
		_status |= BODY_READY;
		return (true);
	}

	bool	closed() {
		if (!(_status & CLOSED)) {
			Headers::const_iterator cit = _headers.find("connection");
			if (it != _headers.end()) {
				if (it->second == "close") {
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
		return (_UNKNOWN);
	}

	/* get method */
	/* GET */
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

	/* get uri */
	/* / */
	bool	_extract_uri() {
		size_t const tmp = _raw.find(" ", _separator_pos);
		if (tmp == std::string::npos) //no separator
			return (_invalid(BAD_REQUEST));
		if (tmp > 8192) /* Apach standard */
			return (_invalid(REQUEST_URI_TOO_LONG));
		_uri = _raw.substr(_separator_pos, tmp);

		if (_uri == "" || _uri[0] != '/') // (trully) not certain about the last condition... 
			return (_invalid(BAD_REQUEST));
		/* uptdate _raw */
		//		_raw_request.erase(0, separator_pos + 1);
		_separator_pos = tmp + 1;

		/* get the _query if there is one */
		if (_uri.find("?", _separator_pos) != std::string::npos) { //so far I undestand that query start after a '?' 
			_query = _uri.substr(_uri.find("?") + 1);
			//_uri.erase(_uri.find("?")); //not sure what to do
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
		_version = _raw.substr(_separator_pos, tmp);
		//_strtolower(&_version); /* need to verify is this is need */
		/* this way is maybe to specific... not sure about the protocol liberty */
		if (_version.find("HTTP/") != 0)
			return (_invalid(BAD_REQUEST));
		if (_version[6] != '1' || _version[7] != '.' || _version[8] != '1')
			return _invalid(HTTP_VERSION_NOT_SUPPORTED);

		/* les restrictiv way... aborded for the moment  --------------.
		   if (_version == "" || _version.find("http", tmp) != 0
		   || _version[4] != '/')
		   return (_invalid(BAD_REQUEST));
		   _http_version = _http_version.substr(_version + 5);
		   if (_http_version != "1.1")
		   return _invalid(HTTP_VERSION_NOT_SUPPORTED);
		   ----------------------*/
		//		_raw_request.erase(0, version_separator_pos + 2);
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
			_strtolower(&header_value); //maybe not
			_trim(&header_value);
			(*bucket)[header_name] = header_value;
			if (header_name == "host")
				_host = header_value;
			_separator_pos = tmp + 2;
			header_size += _separator_pos;
			if (header_size > 16384) /* default 16Kb */
				return (_invalid(REQUEST_HEADER_FIELDS_TOO_LARGE));
			 tmp =  raw.find("/r/n", _separator_pos);
		}
		_raw.erase(0, _separator_pos + 2); //sould erase all the header
		////////////////////////////////////////////////
		std::cout << "test: " << _raw << "\n";
		///////////////////////////////////////////////
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
//				_raw_request.erase(0, header_end + 2);
				if (chunk_size == 0) {
					_raw = body;
					return (READ_OK);
				}
				body += _raw.substr(chunk_size_info + 2, chunk_size);
				_raw_request.erase(0, chunk_size + chunk_size_info + 4);
			} while (true);
		}
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

