/**
 * @author      : abaudot (aimebaudot@gmail.com)
 * @file        : LocationBlock
 * @created     : Monday Mar 28, 2022 15:13:13 CEST
 */

#ifndef LOCATIONBLOCK_HPP
# define LOCATIONBLOCK_HPP

# include "enum.hpp"

# include <unistd.h>
# include <string>
# include <iostream>
# include <map>
# include <algorithm>
# include <vector>


/*
 *  --------------------------------------------
 * | Alina's Serv_block structure                 |
 *  --------------------------------------------
 */
class LocationBlock;

struct Serv_block
{
	typedef std::map<int, std::string>				ErrorObject;
	typedef std::map<std::string, LocationBlock*>	LocationObject;

	ErrorObject		error_page;
	LocationObject	locations;

	std::vector<std::string> index;
	std::vector<bool>	allow;
	std::string			root;
	std::string			server_name;
	std::string			ip;
	std::string			_listen; // <- tmp maybe
	std::string			redirect_path;
	size_t				client_max_body_size;
	int					port;
	int					redirect_num;
	bool				autoindex;

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
		client_max_body_size(other.client_max_body_size),
		port(other.port),
		redirect_num(other.redirect_num),
		autoindex(other.autoindex){}

	/////// only for debuging /////////////
	friend std::ostream &operator<<(std::ostream &ostream_obj, const Serv_block &obj);
};

/*
 *  --------------------------------------------
 * | Alina's Loc_block structure                |
 *  --------------------------------------------
*/

struct Loc_block
{
	typedef std::map<std::string, std::string>	CgiObject;
	typedef std::vector<std::string>			IndexObject;

	std::map<std::string, std::string> cgi_map;
	std::string					path;
	std::string					upload_pass;
	std::string					auth_basic_user_file;
    std::string					root;
	std::string					redirect_path;
    IndexObject					index;
    std::vector<bool>			methods;
	size_t						client_max_body_size;
    int							redirect_num;
	bool    					autoindex;
	bool						auth_basic;

	/* whithoud cgi atm */
	Loc_block(){}

	explicit Loc_block ( Serv_block const &serv ):
			path(),
			upload_pass(),
			auth_basic_user_file(),
			root(serv.root),
			redirect_path(serv.redirect_path),
			index(serv.index),
			methods(serv.allow),
			client_max_body_size(serv.client_max_body_size),
			redirect_num(serv.redirect_num),
			autoindex(serv.autoindex),
			auth_basic(){}

	Loc_block ( Loc_block const &other ):
		path(other.path),
		upload_pass(other.upload_pass),
		auth_basic_user_file(other.auth_basic_user_file),
		root(other.root),
		redirect_path(other.redirect_path),
		index(other.index),
		methods(other.methods),
		client_max_body_size(other.client_max_body_size),
		redirect_num(other.redirect_num),
		autoindex(other.autoindex),
		auth_basic(other.auth_basic){}

	/////// only for debuging /////////////
	friend std::ostream &operator<<(std::ostream &ostream_obj, const Loc_block &obj);
};

/* ---------------------------------------- */

class LocationBlock: private Loc_block
{
	public:
		using Loc_block::IndexObject;

		/* constructor */
		explicit LocationBlock ( Loc_block const &block ):
			Loc_block(block){}

		explicit LocationBlock( Serv_block const &serv ):
			Loc_block(serv){}


		/* destructor */
		~LocationBlock (){} //tmp

		/* getter */
		IndexObject const &get_indexs() const { return (index); }

		std::string const &get_path() const { return (path); }
		std::string const &get_upload_pass() const { return (upload_pass); }
		std::string const &get_root() const { return (root); }
		std::string const &get_redirection() const { return (redirect_path); }

		size_t	get_body_limit() const { return (client_max_body_size); }
		int	get_redirection_code() const { return (redirect_num); }
		bool	is_allowed( int meth ) const { return (methods[meth]); } //tmp
		bool	get_autoindex() const { return (autoindex); }

		// debug //
		std::vector<bool> const &get_methods() const { return (methods); }

	private:
		/* private methods */
};

#endif /* end of include guard LOCATIONBLOCK_HPP */

