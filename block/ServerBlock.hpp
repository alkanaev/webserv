/**
 * @author      : abaudot (aimebaudot@gmail.com)
 * @file        : ServerBlock
 * @created     : Monday Mar 28, 2022 13:36:42 CEST
 */

#ifndef SERVERBLOCK_HPP
# define SERVERBLOCK_HPP

# include "LocationBlock.hpp"


class ServerBlock: protected Serv_block
{
	typedef std::map<std::string, ServerBlock*>	VHostsObject;

	VHostsObject _vhosts; //other server whit the same host name (<host>:port)
	LocationBlock *_defLoc;

	public:
	/* constructor */
	explicit ServerBlock ( Serv_block const &block ):
		Serv_block(block),
		_defLoc(new LocationBlock(block)){}

	ServerBlock ( ServerBlock const &other ):
		Serv_block(other),
		_vhosts(other._vhosts),
		_defLoc(other._defLoc){}


	//default location

	/* destructor (TODO) */
	virtual ~ServerBlock (){
		delete _defLoc;
	}

	/* getter */
	std::string const &get_name() const { return (server_name); }
	std::string const &get_ip() const { return (ip); }
	std::string const &get_root() const { return (root); }
	std::string const &get_redirection() const { return (redirect_path); }
	std::vector<bool> const &get_allow() const { return (allow); }
	std::vector<std::string> const &get_indexs() const { return (index); }
	int	get_redirNUm() const { return (redirect_num); }
	int	get_port() const { return (port); }
	bool get_autoindex() const { return (autoindex); }

	// -----------------for debug Only -------------------------------//
	LocationObject const &get_locations() const { return (locations); }
	size_t get_body_limit() const { return client_max_body_size; }
	ErrorObject const &get_error_page() const { return error_page; }
	std::string const &get_listen() const { return (_listen); }
	// ------------------------------------------------------------

	/* Error pages */
	/*
	   std::string const
	   get_error_page( int status, std::string const &uri ) const {
	   LocationBlock *loc = get_location(uri);
	   if (loc)
	   return loc->get_error_page(status);
	//			return ::get_error_page(status); /// DEFAULT
	}
	*/

	std::string const //no uri then
		get_error_page( int status, std::string const &host ) const {
			ServerBlock const *server = _get_vhost(host);
			if (server)
				return (server->get_error_page(status));
			return (get_error_page(status));
		}

	// shoud be private
	std::string const get_error_page( int code ) const {
		ErrorObject::const_iterator it = error_page.find(code);
		if (it != error_page.end())
			return (it->second);
		return ("");
	}

	const LocationBlock *get_lockBlock( std::string const &host,
			std::string const &uri ) const {
		ServerBlock const *vhost = _get_vhost(host);
		return (vhost->_get_lockBlock(uri));
	}

	void absorb( ServerBlock *lhs ) {
		std::string name = lhs->get_name();
		_vhosts.insert(std::pair<std::string, ServerBlock *>(
					_strtolower(name), lhs));
	}

	private: /* methodes */

	const ServerBlock	*_get_vhost( std::string const &host ) const {
		if (!_vhosts.size())
			return (this);
		std::string real_host = host;
		int port = {-1};
		if (real_host.find(':') != std::string::npos) {
			real_host = real_host.substr(0, real_host.find(':'));
			port = atoi(host.substr(host.find(':') + 1).c_str());
		}
		VHostsObject::const_iterator it = _vhosts.find(real_host);
		if (it != _vhosts.end() && (port == -1 || port == it->second->get_port())) // -1 ?
			return (it->second);
		return (this);
	}

	const LocationBlock	 *_get_lockBlock( std::string const &uri ) const {
		std::string _uri = uri;
		size_t tmp = uri.find("/", 1);
		if (tmp != std::string::npos)
			_uri = _uri.substr(0, tmp);
		LocationObject::const_iterator it = locations.find(_uri);
		if (it != locations.end()) {
			std::cout << " [🚪] in: " << _uri << "\n";
			return (it->second);
		}
		std::cout << " [🏞️] staying at root\n";
		return (_defLoc); //change it for default_lockation :)
	}

	/* remove one day... cleaning */
	std::string &_strtolower( std::string &s ) {
		for (std::string::iterator it = s.begin(); it != s.end(); it++)
			*it = std::tolower(*it);
		return (s);
	}
};

#endif /* end of include guard SERVERBLOCK_HPP */

