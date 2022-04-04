/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf_main.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alkanaev <alkanaev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/27 12:45:17 by alkanaev          #+#    #+#             */
/*   Updated: 2022/04/04 16:01:46 by alkanaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

void Configurations::kick_bad_methods(std::vector<std::string> tokens )
{
	static std::string const words[11] = 
	{"GET", "PUT", "POST", "HEAD", "DELETE", "HEAD", "PUT", "CONNECT", "OPTIONS", "TRACE"};
	for (size_t i = 0; i < tokens.size(); ++i)
		if (std::find(words, words + 11, tokens[i]) == (words + 11))
			err_message("Bad parameter, problem: bad method given");
}

void Configurations::allow_methods(std::string const &directive, int k) 
{
	std::vector<std::string> tokens = split(directive, ',');
	kick_bad_methods(tokens);
	if (k) {
		for (size_t i = 0; i < tokens.size(); ++i) {
			if (tokens[i] == "GET")
				serv.allow[_GET] = true;
			else if (tokens[i] == "POST")
				serv.allow[_POST]= true;
			else if (tokens[i] == "DELETE")
				serv.allow[_DELETE] = true;
		}
	} else {
		for (size_t i = 0; i < tokens.size(); ++i) {
			if (tokens[i] == "GET")
				loc.methods[_GET] = true;
			else if (tokens[i] == "POST")
				loc.methods[_POST] = true;
			else if (tokens[i] == "DELETE")
				loc.methods[_DELETE] = true;
		}
	}
}

std::string Configurations::get_directive(std::string str) 
{
	std::string directive;
	size_t str_len = str.length();
	size_t conf_len = p_config.length() - 1;
	for(; str_len < conf_len; str_len++) 
		directive += p_config[str_len];
	return directive;
}

void Configurations::parser_conf(std::string file)
{
	std::ifstream the_file(file);
	while(getline(the_file, str_to_parse)) 
	{
		size_t len = str_to_parse.length();
		for (size_t i = 0 ; i < len; i++) 
		{
			if (str_to_parse[i] != ' ' && str_to_parse[i] != '\t') 
				str_to_conf += str_to_parse[i];
		}
	}
	the_file.close();
}

int Configurations::line_control() 
{
	p_config.clear();
	size_t len = str_to_conf.length();
	static size_t i = 0;
	if (i == len)
		return 0;
	for(; i < len; i++) 
	{
		if (str_to_conf[i] != '}' && str_to_conf[i] != '{' && str_to_conf[i] != ';') 
			p_config += str_to_conf[i];
		else if (str_to_conf[i] == '}' || str_to_conf[i] == '{' || str_to_conf[i] == ';') 
		{
			p_config += str_to_conf[i];
			i++;
			break ;
		}
	}
	return 1;
}

void Configurations::get_ip(std::string directive) 
{
	int the_port = 0;
	int len = 0;
	int f_pointer = 0;
	std::string tmp;
	for (size_t i = 0; i < directive.length(); i++) 
	{
		if ((directive[i] >= '0' && directive[i] <= ':')
				|| (directive[i] == '.' && the_port == 0)) 
		{
			if(directive[i] == '.') 
			{
				if(f_pointer > 3 || (len > 3 || len < 1))
					err_message("Bad parameter, directive's name: listen");
				f_pointer++;
				len = -1;
			}
			if (directive[i] == ':' && the_port == 0) 
			{
				serv.ip = tmp;
				tmp.clear();
				the_port = 1;
				continue;
			}
			len++;
			tmp += directive[i];
		}
		else
			err_message("Bad parameter, directive's name: listen");
	}
	if (serv.ip.length() == 0) 
	{
		// std::cout << "IP will be set to: 127.0.0.1" << std::endl;
		serv.ip = "127.0.0.1";
	}
	if (tmp.length() > 0) 
		serv.port = ft_stoi_unsign(tmp);
	else
		err_message("Bad parameter, problem: no port (necessary parameter)");
}

int Configurations::get_err_num(std::string const &s)
{
	std::string::size_type pos = s.find('/');
	if (pos != std::string::npos)
		return ft_stoi_unsign(s.substr(0, pos));
	else
		return ft_stoi_unsign(s);
}

std::string Configurations::get_the_path(std::string const &s)
{
	std::string res;
	res = s.substr(s.find("/"));
	return res;
}

int Configurations::check_err_num_path(std::string str) 
{
	std::list<int> errs;
	for (int i = 400; i < 419; i++)
		errs.push_back(i);
	for (int i = 421; i < 427; i++)
		errs.push_back(i);
	errs.push_back(428);
	errs.push_back(429);
	errs.push_back(431);
	errs.push_back(451);
	for (int i = 500; i < 509; i++)
		errs.push_back(i);
	errs.push_back(510);
	errs.push_back(511);

	//Bad parameter, directive's name: root / server
    //errorpages/404.html,404
	// check number
	std::string num = str.substr(0, 3);
	// check path
	std::string path = str.substr(3, str.length() - 1);
	bool found = (std::find(std::begin(errs), std::end(errs), ft_stoi_unsign(num)) != std::end(errs));
	std::string::const_iterator it = num.begin();
	while (it != num.end() && std::isdigit(*it))
		++it;
	if ((!num.empty() && it == num.end()) && found && check_string(path, 0))
		return 1;
	return 0;
}

int Configurations::check_redirect(std::string str)
{
	// if I understood right - can be any code given. 
	// But there are the most popular code in case of redirection : 301, 302, 404.
	// Will we filter the errors ?
	std::string num = str.substr(0, 3);
	std::string::const_iterator it = num.begin();
	while (it != num.end() && std::isdigit(*it))
		++it;
	if ((!num.empty() && it == num.end()) && str.length() > 3)
		return 1;
	return 0;
}

std::map<int,std::string> Configurations::take_error_pages(std::string directive)
{
	if (!check_err_num_path(directive))
	{
		std::cout << directive << "\n";
		err_message("Bad parameter, directive's name: error_page");
	}
	else
		serv.error_page[get_err_num(directive)] = get_the_path(directive);
	return serv.error_page;
}

void Configurations::take_redirect(std::string directive, int k)
{
	if (!check_redirect(directive))
		err_message("Bad parameter, problem of redirect parameter");
	else
	{
		int num = ft_stoi_unsign(directive.substr(0, 3));
		std::string path = directive.substr(3, directive.length() - 1);
		if (k)
		{
			serv.redirect_num = num;
			serv.redirect_path = path;
		}
		else 
			loc.redirect_num = num;
		loc.redirect_path = path;
	}
}


void Configurations::take_index_vector(std::string directive, int k)
{
	std::string tmp = directive;
	std::string sub;
	std::string delimiter = ".html";
	size_t pos;
	std::string token;
	while ((pos = tmp.find(delimiter)) != std::string::npos) 
	{
		token = tmp.substr(0, pos);
		if (k)
			serv.index.push_back(token.insert(token.length(), delimiter));
		else
			loc.index.push_back(token.insert(token.length(), delimiter));
		tmp.erase(0, pos + delimiter.length());
	}
}

std::string Configurations::get_absolut() 
{
   char buff[40];
   getcwd(buff, 40);
   std::string cwd(buff);
   return cwd;
}

void Configurations::take_server_directives(std::string name , std::string directive) 
{
	if (name == "listen") 
	{
		get_ip(directive);
		serv._listen = directive;
	} 
	else if (name == "return")
		take_redirect(directive, 1);
	else if (name == "root")
	{
		serv.root = get_absolut() + directive;
		if (!check_string(directive, 0))
		{
			std::cout << "directive: " << directive << "\n";
			err_message("Bad parameter, directive's name: root / server");
		}
	}
	else if (name == "allow") 
		allow_methods(directive, 1);
	else if (name == "index")
		take_index_vector(directive, 1);
	else if(name == "error_page")
		serv.error_page = take_error_pages(directive);
	else if (name == "autoindex") 
	{
		if (directive == "on")
			serv.autoindex = true;
		else if (directive == "off") 
			serv.autoindex = false;
		else
			err_message("Bad parameter, directive's name: autoindex / server");
	}
	else if (name == "server_name") 
		serv.server_name = directive;
	else if (name == "client_max_body_size")
	{
		std::string::const_iterator it = directive.begin();
		while (it != directive.end() && std::isdigit(*it))
			++it;
		if ((!directive.empty() && it == directive.end()))
			serv.client_max_body_size = ft_stoi_unsign(directive);

		if ((it != directive.end()) && (next(it) == directive.end()))
		{
			char check = directive.back();
			if (check == 'M')
			{
				directive.pop_back();
				serv.client_max_body_size = ft_stoi_unsign(directive.append("000000"));
			}
			else
				err_message("Bad parameter, value of client_max_body_size isn't valid");
		}
	}
	else if (name == "location")
	{
		loc.path = directive;
		
		if (!check_string(directive, 1))
			err_message("Bad parameter, problem: location path");
	}
}

std::string Configurations::get_cgi_ext(std::string const &s)
{
	std::string::size_type pos = s.find('/');
	if (pos != std::string::npos)
		return s.substr(0, pos);
}

void Configurations::take_location_directives(std::string name, std::string directive) 
{
	if (name == "root")
	{
		loc.root = serv.root + directive;
		std::cout << loc.root << std::endl;
		if (!check_string(directive, 0))
			err_message("Bad parameter, directive's name: root / location");
	}
	else if (name == "return") 
		take_redirect(directive, 0);
	else if (name == "methods") 
		allow_methods(directive, 0);
	else if (name == "index") 
		take_index_vector(directive, 0);
	else if (name == "autoindex") 
	{
		if (directive == "on") 
			loc.autoindex = true;
		else if (directive == "off") 
			loc.autoindex = false;
		else
			err_message("Bad parameter, directive's name: autoindex / location");
	} 
	else if (name == "auth_basic")
	{
		if (directive == "on")
			loc.auth_basic = true;
		if (directive == "off")
			loc.auth_basic = false;
	}
	else if (name == "auth_basic_user_file") 
		loc.auth_basic_user_file = directive;
	// else if (name == "cgi_extension")
	// 	loc.cgi_extension.push_back(directive);
	// else if (name == "cgi_path")
	// {
	// 	if (!loc.cgi_path.length())
	// 		loc.cgi_path = directive;
	// 	else
	// 		err_message("Bad parameter, cgi: only one cgi path per location is allowed");
	// }
	else if (name == "client_max_body_size")
		loc.client_max_body_size = ft_stoi_unsign(directive);
	else if (name == "upload_pass")
	{
		loc.upload_pass = serv.root + directive;
		if (!check_string(directive, 0))
			err_message("Bad paportmeter, directive's name: root / location");
	}
	if (name == "cgi")
	{
		// std::cout << loc.cgi_path << std::endl;
		// for (int i = 0; i < loc.cgi_extension.size(); i++) 
		// {
			//  std::cout << loc.cgi_extension.at(i) << " -*- ";
		// }
		std::string cgi_ext = get_cgi_ext(directive);
		std::string cgi_path = get_the_path(directive);
		if (cgi_ext.length() && cgi_path.length())
			loc.cgi_map[cgi_path] = cgi_ext;
	}
}

void Configurations::config_part() {
	int searching_checker = 0;
	serv_init();
	for(std::vector<std::string>::iterator itc = directives_config.begin(); itc != directives_config.end(); itc++) 
	{
		if (p_config.find(*itc) == 0) {
			if (check_end_line('{')) {
				server_block = 1;
				searching_checker = 1;
				break;
			}	
		}
	}
	if (!searching_checker) 
	{
		std::cout << "Bad string " << err_str_set_get(1)
			<< ". Directives outside of a server-context are not allowed\n" << std::endl;
		error_check_point = 1;
	}
}

void Configurations::take_server_part()
{
	loc_init(0);
	if (p_config == "}")
	{ 
		server_block = 0;
		server.push_back(new ServerBlock(serv)); //modif done
	}
	else 
	{
		for(std::vector<std::string>::iterator its = directives_serv.begin(); its != directives_serv.end(); its++) 
		{
			if (p_config.find(*its) == 0) 
			{
				if (*its == "location") 
				{
					if (check_end_line('{')) 
					{
						take_server_directives(*its, get_directive(*its));
						location_block = 1;
					}
				} 
				else if (check_end_line(';')) 
				{
					take_server_directives(*its, get_directive(*its));
					break;
				}	
			}
		}
	}
}

void Configurations::take_location_part()
{
	if (p_config == "}") 
	{
		location_block = 0;
		serv.locations[loc.path] = new LocationBlock(loc);
		// all the nex loc struct we push to serv.location
		//serv.location.push_back(loc); no need anymore
	}
	else 
	{
		for(std::vector<std::string>::iterator itl = directives_loc.begin(); itl != directives_loc.end(); itl++) 
		{
			if (!p_config.find(*itl)) 
			{
				if (check_end_line(';')) 
					take_location_directives(*itl, get_directive(*itl));
			}
		}
	}
}

std::string Configurations::get_line() 
{
	return p_config;
}

int Configurations::err_str_set_get(int k) 
{
	if (k)
		return error_str_cnt;
	error_str_cnt += 1;
	return 0;
}

void Configurations::work(std::string file) 
{
	general_init();
	serv_init();
	loc_init(1);
	parser_conf(file);
	while(line_control()) 
	{
		err_str_set_get(0);
		if (get_line()[0] == '#') 
			continue;
		else if (!is_server_block())
			config_part();
		else if (is_server_block() && !is_location_block())
			take_server_part();
		else if (is_server_block() && is_location_block())
			take_location_part();
	}
	if (error_found() == 1) 
		std::cout << "[💥] The configuration file is not accepted !!!" << std::endl;
	else 
		std::cout << "[☀️]] The configuration file accepted!\n";
}
