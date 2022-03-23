#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <list>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>

enum  Allowed
{
	GET = 0,
	PUT = 1,
	POST = 2,
	HEAD = 3,
	DELETE = 4,
	UNSUPPORTED = 5
};

struct Loc_block
{
	std::vector<Allowed>	methods;
	std::string    path;
    std::vector<std::string> index;
	bool    autoindex;
	std::string    root;
	bool	auth_basic;
	std::string	auth_basic_user_file;
    std::string	cgi_path; // transform to std::map<std::string, std::string>
	std::string	cgi_extension;
	unsigned int    client_max_body_size;

	friend std::ostream &operator<<(std::ostream &ostream_obj, const Loc_block &obj);
};

struct Serv_block
{
	std::vector<Allowed> 	allow;
	std::vector<Loc_block>	location;
	int    port;
	std::string    ip;
	std::string    listen;
    std::vector<std::string> index;
	bool	autoindex;
	std::string	root;
	std::string    server_name;
    std::map<int,std::string> error_page;
	
	friend std::ostream &operator<<(std::ostream &ostream_obj, const Serv_block &obj);
};

class Configurations
{
	public:
        std::vector<std::string> split(const std::string &str, char sep);
        void	general_init();
        void	serv_init();
        void    loc_init(int k);
        int    is_server_block();
        int    is_location_block();
        std::string	get_line();
        int check_string(std::string str);
        int check_err_num_page(std::string str);
        void allow_methods(std::string directive, int k);
        unsigned int	ft_stoi_unsign(std::string);
        void    parser_conf(std::string file);
        int    line_control();
        void    config_part();
        void    take_server_part();
        void    take_location_part();
        std::string    get_directive(std::string str);
        void    get_ip(std::string parametr);
        void    take_server_directives(std::string name , std::string directive);
        void    take_location_directives(std::string name , std::string directive);
        int    check_end_line(char i);
        void    work(std::string file);
        void    print_parsed();
        std::map<int,std::string> take_error_pages(std::string directive);
        int get_err_num(std::string const &s);
        std::string get_err_path(std::string const &s);
        void take_index_vector(std::string directive, int k);
        void kick_bad_methods(std::vector<std::string> tokens, std::string words[6]);
        void err_message(std::string str);
        int error_found();
        int	err_str_set_get(int k);

        friend std::ostream &operator<<(std::ostream &ostream_obj, const Configurations &obj);
        friend std::ostream &operator<<(std::ostream &ostream_obj, const Serv_block &obj);
        friend std::ostream &operator<<(std::ostream &ostream_obj, const Loc_block &obj);

    private:
        Serv_block    serv;
        Loc_block    loc;
        int    server_block;
        int    location_block;
        int f_error_str;
        int f_error;
        std::string str_to_parse;
        std::string	str_to_conf;
        std::string	p_config;
        std::vector<std::string>	directives_config;
        std::vector<std::string>	directives_serv;
        std::vector<std::string>	directives_loc;
        std::vector<Serv_block*>	server;
        std::map<std::string, Loc_block> loc_map;
};

#endif