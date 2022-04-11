/**
 * @author      : abaudot (aimebaudot@gmail.com)
 * @file        : enum
 * @created     : Thursday Mar 17, 2022 19:19:22 CET
 */

#ifndef ENUM_HPP
# define ENUM_HPP
// ----------- colors ------------
# define	BLACK	"\033[1;30m"
# define	RED		"\033[1;31m"
# define	GREEN	"\033[1;32m"
# define	YELLOW	"\033[1;33m"
# define	BLUE	"\033[1;34m"
# define	CYAN	"\033[1;35m"
# define	PURPLE	"\033[1;36m"
# define	WHITE	"\033[1;37m"
# define	EOCC	"\033[0;0m"
# define	EOC		"\033[0;0m\n"
// --------------------------------<

enum METHODS {
	_GET, // retrive info from server using uri
	_POST, // send message to the server
	_DELETE, // remove all the current representation of the target ressource given by uri
	/* ------- NON IMPLEMENTED --------
	   _HEAD, // GET + transfers the status line and the header section only
	   _PUT, // replace all the current representation of target ressource whith the uploaded content
	   _CONNECT, // Etablishes a tunnel to the server identidied by a ginven uri
	   _OPTIONS, // Describe the communication options for the target ressource
	   _TRACE, // Performs a messge loop back test along with the path to the target ressource
	   */
	METHOD_UNKNOWN /* undefin|default method */
};

enum READ {
	READ_OK,
	READ_EOF,
	READ_ERROR,
	READ_WAIT
};

enum FORM {
	_URLENCODED,
	_MULTIPART,
	_PLAINTXT,
	FORM_UNKNOWN
};

enum STATUS_CODE {
	CONTINUE = 100,
	SWITCHING_PROTOCOLS = 101,
	PROCESSING = 102,
	EARLY_HINTS = 103,
	OK = 200,
	CREATED = 201,
	ACCEPTED = 202,
	NON_AUTHORITATIVE_INFORMATION = 203,
	NO_CONTENT = 204,
	RESET_CONTENT = 205,
	PARTIAL_CONTENT = 206,
	MULTI_STATUS = 207,
	ALREADY_REPORTED = 208,
	IM_USED = 226,
	MULTIPLE_CHOICES = 300,
	MOVED_PERMANENTLY = 301,
	FOUND = 302,
	SEE_OTHER = 303,
	NOT_MODIFIED = 304,
	USE_PROXY = 305,
	SWITCH_PROXY = 306,
	TEMPORARY_REDIRECT = 307,
	PERMANENT_REDIRECT = 308,
	BAD_REQUEST = 400,
	UNAUTHORIZED = 401,
	PAYMENT_REQUIRED = 402,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	NOT_ACCEPTABLE = 406,
	PROXY_AUTHENTICATION_REQUIRED = 407,
	REQUEST_TIMEOUT = 408,
	CONFLICT = 409,
	GONE = 410,
	LENGTH_REQUIRED = 411,
	PRECONDITION_FAILED = 412,
	PAYLOAD_TOO_LARGE = 413,
	REQUEST_URI_TOO_LONG = 414,
	UNSUPPORTED_MEDIA_TYPE = 415,
	REQUESTED_RANGE_NOT_SATISFIABLE = 416,
	EXPECTATION_FAILED = 417,
	IM_A_TEAPOT = 418,
	MISDIRECTED_REQUEST = 421,
	UNPROCESSABLE_ENTITY = 422,
	LOCKED = 423,
	FAILED_DEPENDENCY = 424,
	UPGRADE_REQUIRED = 426,
	PRECONDITION_REQUIRED = 428,
	TOO_MANY_REQUESTS = 429,
	REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
	UNAVAILABLE_FOR_LEGAL_REASONS = 451,
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	BAD_GATEWAY = 502,
	SERVICE_UNAVAILABLE = 503,
	GATEWAY_TIMEOUT = 504,
	HTTP_VERSION_NOT_SUPPORTED = 505,
	VARIANT_ALSO_NEGOTIATES = 506,
	INSUFFICIENT_STORAGE = 507,
	LOOP_DETECTED = 508,
	NOT_EXTENDED = 510,
	NETWORK_AUTHENTICATION_REQUIRED = 511
};
#endif /* end of include guard ENUM_HPP */

