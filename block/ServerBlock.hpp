/**
 * @author      : abaudot (aimebaudot@gmail.com)
 * @file        : ServerBlock
 * @created     : Monday Mar 28, 2022 13:36:42 CEST
 */

#ifndef SERVERBLOCK_HPP
# define SERVERBLOCK_HPP

# include "LocationBlock.hpp"

/*
 *  --------------------------------------------
 * | Alina's Serv_block structure                 |
 *  --------------------------------------------
*/
struct Serv_block
{
	typedef std::map<int, std::string>				ErrorObject;
	typedef std::map<std::string, LocationBlock*>	LocationObject;

	ErrorObject		error_page;
	// will change loc_map at some point
//    std::map<std::string, Loc_block> loc_map;
	LocationObject	locations;

//	std::vector<Loc_block>	location; <- no need any more
    std::vector<std::string> index;
    std::vector<bool>	allow;
	std::string			root;
	std::string			server_name;
	std::string			ip;
	std::string			_listen; // <- tmp maybe
    std::string			redirect_path;
	int					port;
    int					redirect_num;
	bool				autoindex;
    unsigned int    	client_max_body_size;
	
	Serv_block (){} //tmp

	Serv_block ( Serv_block const &other ):
		error_page(other.error_page),
		locations(other.locations),
		index(other.index),
		allow(other.allow),
		root(other.root),
		server_name(other.server_name),
		ip(other.ip),
		_listen(other._listen), //remove it
		redirect_path(other.redirect_path),
		port(other.port),
		redirect_num(other.redirect_num),
		autoindex(other.autoindex),
		client_max_body_size(other.client_max_body_size){}
	
	/////// only for debuging /////////////
	friend std::ostream &operator<<(std::ostream &ostream_obj, const Serv_block &obj);
};

class ServerBlock: protected Serv_block
{
	typedef std::map<std::string, ServerBlock*>	VHostsObject;

	VHostsObject _vhosts; //other server whit the same host name (<host>:port)

	public:
		/* constructor */
		explicit ServerBlock ( Serv_block const &block ):
			Serv_block(block){}

		ServerBlock ( ServerBlock const &other ):
			Serv_block(other),
			_vhosts(other._vhosts){}

		//default location

		/* destructor (TODO) */
		virtual ~ServerBlock (){}

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
			if (uri.find("/", 1) != std::string::npos)
				_uri = _uri.substr(0, uri.find("/", 1));
			LocationObject::const_iterator it = locations.find(_uri);
			if (it != locations.end())
				return (it->second);
			return (0); //change it for default_lockation :)
	}

	/* remove one day... cleaning */
	std::string &_strtolower( std::string &s ) {
		for (std::string::iterator it = s.begin(); it != s.end(); it++)
			*it = std::tolower(*it);
		return (s);
	}
};

#endif /* end of include guard SERVERBLOCK_HPP */

