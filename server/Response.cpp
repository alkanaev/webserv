/**
 * @author      : abaudot (aimebaudot@gmail.com)
 * @file        : Response
 * @created     : Tuesday Mar 29, 2022 14:25:39 CEST
 */

#include "Response.hpp"

Response::StatusMap Response::_initStatusMap() {

	//debug 
	std::cout << "init_Status\n";
	Response::StatusMap statusCode;

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
	statusCode[IM_A_TEAPOT] = "I'm a teapot"; //lol
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
	return (statusCode);
}

Response::TypeMap Response::_initTypeMap( void ) {

	//debug//
	std::cout << "init typesMAP\n";

	Response::TypeMap	mime_types;
	mime_types["aac"] = "audio/aac";
	mime_types[".abw"] = "application/x-abiword";
	mime_types[".arc"] = "application/x-freearc";
	mime_types[".avi"] = "video/x-msvideo";
	mime_types[".azw"] = "application/vnd.amazon.ebook";
	mime_types[".bin"] = "application/octet-stream";
	mime_types[".bmp"] = "image/bmp";
	mime_types[".bz"] = "application/x-bzip";
	mime_types[".bz2"] = "application/x-bzip2";
	mime_types[".cda"] = "application/x-cdf";
	mime_types[".csh"] = "application/x-csh";
	mime_types[".css"] = "text/css";
	mime_types[".csv"] = "text/csv";
	mime_types[".doc"] = "application/msword";
	mime_types[".docx"] =
		"application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	mime_types[".eot"] = "application/vnd.ms-fontobject";
	mime_types[".epub"] = "application/epub+zip";
	mime_types[".gz"] = "application/gzip";
	mime_types[".gif"] = "image/gif";
	mime_types[".htm"] = "text/html";
	mime_types[".html"] = "text/html";
	mime_types[".ico"] = "image/vnd.microsoft.icon";
	mime_types[".ics"] = "text/calendar";
	mime_types[".jar"] = "application/java-archive";
	mime_types[".jpeg"] = "image/jpeg";
	mime_types[".jpg"] = "image/jpeg";
	mime_types[".js"] = "text/javascript";
	mime_types[".json"] = "application/json";
	mime_types[".jsonld"] = "application/ld+json";
	mime_types[".mid"] = "audio/midi";
	mime_types[".midi"] = "audio/x-midi";
	mime_types[".mjs"] = "text/javascript";
	mime_types[".mp3"] = "audio/mpeg";
	mime_types[".mp4"] = "video/mp4";
	mime_types[".mpeg"] = "video/mpeg";
	mime_types[".mpkg"] = "application/vnd.apple.installer+xml";
	mime_types[".odp"] = "application/vnd.oasis.opendocument.presentation";
	mime_types[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	mime_types[".odt"] = "application/vnd.oasis.opendocument.text";
	mime_types[".oga"] = "audio/ogg";
	mime_types[".ogv"] = "video/ogg";
	mime_types[".ogx"] = "application/ogg";
	mime_types[".opus"] = "audio/opus";
	mime_types[".otf"] = "font/otf";
	mime_types[".png"] = "image/png";
	mime_types[".pdf"] = "application/pdf";
	mime_types[".php"] = "application/x-httpd-php";
	mime_types[".ppt"] = "application/vnd.ms-powerpoint";
	mime_types[".pptx"] =
		"application/vnd.openxmlformats-officedocument.presentationml.presentation";
	mime_types[".rar"] = "application/x-rar-compressed";
	mime_types[".rtf"] = "application/rtf";
	mime_types[".sh"] = "application/x-sh";
	mime_types[".svg"] = "image/svg+xml";
	mime_types[".swf"] = "application/x-shockwave-flash";
	mime_types[".tar"] = "application/x-tar";
	mime_types[".tif"] = "image/tiff";
	mime_types[".tiff"] = "image/tiff";
	mime_types[".ttf"] = "font/ttf";
	mime_types[".ts"] = "video/mp2t";
	mime_types[".txt"] = "text/plain";
	mime_types[".vsd"] = "application/vnd.visio";
	mime_types[".wav"] = "audio/wav";
	mime_types[".weba"] = "audio/webm";
	mime_types[".webm"] = "video/webm";
	mime_types[".webp"] = "image/webp";
	mime_types[".woff"] = "font/woff";
	mime_types[".woff2"] = "font/woff2";
	mime_types[".xhtml"] = "application/xhtml+xml";
	mime_types[".xls"] = "application/vnd.ms-excel";
	mime_types[".xlsx"] =
		"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	mime_types[".xml"] = "application/xml";
	mime_types[".xul"] = "application/vnd.mozilla.xul+xml";
	mime_types[".zip"] = "application/zip";
	mime_types[".3gp"] = "video/3gpp";
	mime_types[".3g2"] = "video/3gpp2";
	mime_types[".7z"] = "application/x-7z-compressed";

	return (mime_types);
}


Response::StatusMap Response::__statusCode = _initStatusMap();

Response::TypeMap Response::__MIMEtypes = _initTypeMap();
