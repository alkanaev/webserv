/**
 * @author      : abaudot (aimebaudot@gmail.com)
 * @file        : LocationBlock
 * @created     : Monday Mar 28, 2022 15:13:13 CEST
 */

#ifndef LOCATIONBLOCK_HPP
# define LOCATIONBLOCK_HPP

# include <unistd.h>
# include <string>
# include <iostream>
# include <map>
# include <algorithm>
# include <vector>

/*
 *  --------------------------------------------
 * | Alina's Loc_block structure                |
 *  --------------------------------------------
*/

struct Loc_block
{
	typedef std::map<std::string, std::string>	CgiObject;

    std::string	cgi_path; // transform to std::map<std::string, std::string> (?)
	std::string	cgi_extension; // need to know that we need exactly these chi directives
	std::string					path;
	std::string					upload_path;
	std::string					auth_basic_user_file;
    std::string					root;
	std::string					redirect_path;
    std::vector<std::string>	index;
    std::vector<bool>			methods;
	size_t						client_max_body_size;
    int							redirect_num;
	bool    					autoindex;
	bool						auth_basic;

	/* whithoud cgi atm */
	Loc_block ( Loc_block &other ):
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
	std::ostream &operator<<(std::ostream &ostream_obj, const Loc_block &obj);
};

class LocationBlock: private Loc_block
{
    public:
		/* constructor */
        explicit LocationBlock (Loc_block const &block):
			Loc_block(block){}

		/* destructor */
        ~LocationBlock (){} //tmp

		/* getter */
		std::vector<std::string> const &get_indexs() const { return (index); }

		std::string const &get_upload_pass() const { return (upload_path); }
		std::string const &get_root() const { return (root); }
		std::string const &get_redirection() const { return (redirect_path); }

		size_t	get_body_limit() const { return (client_max_body_size); }
		int	get_redirection_code() const { return (redirect_num); }
		bool	is_allowed( METHODS const &meth ) cosnt { return (methods[meth]; }

	private:
        /* private methods */
};

#endif /* end of include guard LOCATIONBLOCK_HPP */

