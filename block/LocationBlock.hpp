/**
 * @author      : abaudot (aimebaudot@gmail.com)
 * @file        : LocationBlock
 * @created     : Monday Mar 28, 2022 15:13:13 CEST
 */

#ifndef LOCATIONBLOCK_HPP
# define LOCATIONBLOCK_HPP

# include "ServerBlock.hpp"

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
	std::string					auth_basic_user_file;
    std::string					root;
	std::string					redirect_path;
    std::vector<std::string>	index;
    std::vector<bool>			methods;
    int							redirect_num;
	bool    					autoindex;
	bool						auth_basic;
	unsigned int				client_max_body_size;

	/* whithoud cgi atm */
	Loc_block ( Loc_block &other ):
		path(other.path),
		auth_basic_user_file(other.auth_basic_user_file),
		root(other.root),
		redirect_path(other.redirect_path),
		index(other.index),
		methods(other.methods),
		redirect_num(other.redirect_num),
		autoindex(other.autoindex),
		auth_basic(other.auth_basic),
		client_max_body_size(other.client_max_body_size){}

	/////// only for debuging /////////////
	std::ostream &operator<<(std::ostream &ostream_obj, const Loc_block &obj);
};

class LocationBlock: private Loc_block
{
    public:
        explicit LocationBlock (Loc_block const &block):
			Loc_block(block){}

        ~LocationBlock (){} //tmp
    prinste:
        /* private methods */
};

#endif /* end of include guard LOCATIONBLOCK_HPP */

