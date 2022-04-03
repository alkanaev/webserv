/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alkanaev <alkanaev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/27 12:45:20 by alkanaev          #+#    #+#             */
/*   Updated: 2022/04/03 21:35:13 by alkanaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "ServerBlock.hpp"

#include <stdlib.h>
#include <list> /* list of errors */
#include <fstream>
#include <sstream>

class Configurations
{
	public:
		/* constructor */
		Configurations (void) {};

		Configurations( int ac, char **av ) {
			if (ac != 2)
				throw std::invalid_argument("**Plese, give a path to a config as argument**");
			std::cout << "\n±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±\n";
			std::cout << "[🏊] Parsing our .conf file...\n";
			work(av[1]);
			if (error_found())
				throw std::invalid_argument("Bad Configuration File");
#ifdef DEBUG
			print_parsed();
#endif
		}

		/* payload */
		std::vector<ServerBlock*>	get_server() {
			_handle_vhost();
			return (server);
		}

		~Configurations() {
			std::cout << "±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±\n";
		}

		/* --------- parsing functions -------- */

		std::vector<std::string> split(const std::string &str, char sep);
		void	general_init();
		void	serv_init();
		void    loc_init(int k);
		int    is_server_block();
		int    is_location_block();
		std::string	get_line();
		int check_string(std::string str, int k);
		int check_err_num_path(std::string str);
		void allow_methods(std::string const &directive, int k);
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
		void kick_bad_methods(std::vector<std::string> tokens);
		void err_message(std::string str);
		bool error_found();
		int	err_str_set_get(int k);
		void take_redirect(std::string directive, int k);
		int check_redirect(std::string str);
		std::string get_absolut();

		friend std::ostream &operator<<(std::ostream &ostream_obj, const Configurations &obj);
		friend std::ostream &operator<<(std::ostream &ostream_obj, const Serv_block &obj);
		friend std::ostream &operator<<(std::ostream &ostream_obj, const Loc_block &obj);

	private: /* data */
		Serv_block    serv;
		Loc_block    loc;
		int    server_block;
		int    location_block;
		int error_str_cnt;
		bool  error_check_point;
		std::string str_to_parse;
		std::string	str_to_conf;
		std::string	p_config;
		std::vector<std::string>	directives_config;
		std::vector<std::string>	directives_serv;
		std::vector<std::string>	directives_loc;
		std::vector<ServerBlock*>	server;

		/* method */

		void _handle_vhost() {
			if (server.size() == 0)
				return ;
			std::vector<ServerBlock*>::const_iterator it = server.begin();
			for (; it != server.end(); ++it) {
				std::vector<ServerBlock*>::const_iterator it2 = it + 1;
				for (; it2 != server.end(); it2++) {
					if ((*it)->get_port() == (*it2)->get_port() &&
							(*it)->get_ip() == (*it2)->get_ip()) {
						(*it)->absorb(*it2);
						//delete (*it2);
						server.erase(it2);
						--it2;
					}
				}
			}
		}
};

#endif
