/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conf_init.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alkanaev <alkanaev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/03 18:08:40 by alkanaev          #+#    #+#             */
/*   Updated: 2022/04/05 19:29:16 by alkanaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

void Configurations::general_init() 
{
	error_check_point = 0;
	error_str_cnt = 0;

	server_block = 0;
	directives_config.push_back("server");
	directives_serv.push_back("listen");
	directives_serv.push_back("root");
	directives_serv.push_back("index");
	directives_serv.push_back("autoindex");
	directives_serv.push_back("server_name");
	directives_serv.push_back("error_page");
	directives_serv.push_back("allow");
	directives_serv.push_back("return");
	directives_serv.push_back("location");
	directives_serv.push_back("client_max_body_size");

	location_block = 0;
	directives_loc.push_back("index");
	directives_loc.push_back("autoindex");
	directives_loc.push_back("methods");
	directives_loc.push_back("auth_basic");
	directives_loc.push_back("auth_basic_user_file");
	directives_loc.push_back("root");
	directives_loc.push_back("cgi");
	directives_loc.push_back("client_max_body_size");
	directives_loc.push_back("return");
	directives_loc.push_back("upload_pass");
}

void Configurations::serv_init() 
{ 
	serv.redirect_num = 0;
	serv.redirect_path.clear();
	serv.allow.resize(3, false);
	serv._listen.clear();
	serv.ip.clear();
	serv.root = "/";
	serv.index.clear();
	serv.autoindex = false;
	serv.port = -1;
	serv.server_name.clear();
	serv.client_max_body_size = 100000000;
}

void Configurations::loc_init(int k)
{
	if (k)
	{
		loc.path.clear();
		loc.methods.resize(3, false);
	}
	else
		std::fill(loc.methods.begin(), loc.methods.end(), false);
	loc.root = serv.root;
	loc.redirect_num = 0;
	loc.redirect_path.clear();
	loc.index.clear();
	loc.autoindex = false;
	loc.auth_basic = false;
	loc.auth_basic_user_file.clear();
	loc.client_max_body_size = serv.client_max_body_size;
}