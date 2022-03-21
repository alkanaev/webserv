#include "Config.hpp"
#include <sstream>
 
std::vector<std::string> Configurations::split(const std::string &str, char sep)
{
	std::vector<std::string> elems;
    std::stringstream ss(str);
    std::string part;
    while(std::getline(ss, part, sep)) {
        elems.push_back(part);
    }
    return elems;
}

void Configurations::allow_methods(std::string directive) 
{
	std::string words[6] = {"GET", "PUT", "POST", "HEAD", "DELETE", "UNSUPPORTED"};
	Allowed par[6] = {GET, PUT, POST, HEAD, DELETE, UNSUPPORTED};
	std::string tmp;
	tmp.clear();
	std::string delimiter = ",";
	std::vector<std::string> tokens = split(directive, ',');
	int j = 0;
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < tokens.size(); j++)
		{
			if (tokens[j] == words[i])
			{
				if(std::find(loc.methods.begin(), loc.methods.end(), par[i]) == loc.methods.end())
					loc.methods.push_back(par[i]);
			}
			else 
				tmp += directive[i];
		}
	}
}

void Configurations::allow_methods_serv(std::string directive) 
{
	std::string words[6] = {"GET", "PUT", "POST", "HEAD", "DELETE", "UNSUPPORTED"};
	Allowed par[6] = {GET, PUT, POST, HEAD, DELETE, UNSUPPORTED};
	std::string tmp;
	tmp.clear();
	std::string delimiter = ",";
	std::vector<std::string> tokens = split(directive, ',');
	int j = 0;
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < tokens.size(); j++)
		{
			if (tokens[j] == words[i])
			{
				if(std::find(serv.allow.begin(), serv.allow.end(), par[i]) == serv.allow.end())
					serv.allow.push_back(par[i]);
			}
			else 
				tmp += directive[i];
		}
	}
}

void Configurations::general_init() 
{
	server_block = 0;
	directives_config.push_back("server");
	directives_serv.push_back("listen");
	directives_serv.push_back("root");
	directives_serv.push_back("index");
	directives_serv.push_back("autoindex");
	directives_serv.push_back("server_name");
	directives_serv.push_back("error_page");
	directives_serv.push_back("location");
	directives_serv.push_back("allow");

	location_block = 0;
	directives_loc.push_back("index");
	directives_loc.push_back("autoindex");
	directives_loc.push_back("methods");
	directives_loc.push_back("auth_basic");
	directives_loc.push_back("auth_basic_user_file");
	directives_loc.push_back("root");
	directives_loc.push_back("cgi_extension");
	directives_loc.push_back("cgi_path");
	directives_loc.push_back("client_max_body_size");
}

void Configurations::serv_init() 
{ 
	serv.listen.clear();
	serv.ip.clear();
	serv.root.clear();
	serv.index.clear();
	serv.autoindex = false;
	serv.port = 0;
	serv.server_name.clear();
	serv.error_page.clear();
	serv.location = std::vector<Loc_block>();
}

void Configurations::loc_init(int k)
{
	if (k)
	{
		loc.path.clear();
		loc.methods = std::vector<Allowed>();
	}
	else
	{
		loc.methods.clear();
	}
	loc.root.clear();
	loc.index.clear();
	loc.autoindex = false;
	loc.auth_basic = false;
	loc.auth_basic_user_file.clear();
	loc.cgi_extension.clear();
	loc.cgi_path.clear();
	loc.client_max_body_size = 0;
}

std::string Configurations::get_directive(std::string str) 
{
	std::string directive;
	int str_len = str.length();
	int conf_len = p_config.length() - 1;
	for(; str_len < conf_len; str_len++) {
		directive += p_config[str_len];
	}
	return directive;
}

int Configurations::is_server_block() 
{
	return server_block;
}

int Configurations::is_location_block() 
{
	return location_block;
}

std::string Configurations::get_line() 
{
	return p_config;
}

int Configurations::check_string(std::string str) 
{
	int len = str.length();
	if (str[0] == '/' && str[len - 1] != '/')
		return 1;
	else
		return 0;
}

unsigned int Configurations::ft_stoi_unsign(std::string str) 
{
	unsigned int res = 0;
	int len = str.length();
	for(int i = 0; i < len; i++) {
		if (i != 0) {
			res *= 10;
		}
		res += str[i] - '0';
	}
	return res;
}

void Configurations::parser_conf(std::string file)
{
	int len;
	std::ifstream the_file(file);
	while(getline(the_file, str_to_parse)) 
	{
		len = str_to_parse.length();
		for (int i = 0 ; i < len; i++) 
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
	int len = str_to_conf.length();
	static int i = 0;
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

int Configurations::check_end_line(char i) 
{
	int k = (p_config.length() - 1);
	if (p_config[k] == i) 
		return 1;
	return 0;
}

void Configurations::get_ip(std::string directive) 
{
	int the_port = 0;
	int len = 0;
	std::string tmp;
	for (int i = 0; i < directive.length(); i++) 
	{
		if ((directive[i] >= '0' && directive[i] <= ':')
			|| (directive[i] == '.' && the_port == 0)) 
		{
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
	}
	if (serv.ip.length() == 0) 
	{
		// std::cout << "IP will be set to: 127.0.0.1" << std::endl;
		serv.ip = "127.0.0.1";
	}
	if (tmp.length() > 0) 
		serv.port = ft_stoi_unsign(tmp);
}

void Configurations::take_server_directives(std::string name , std::string directive) 
{
	if (name == "listen") 
	{
		get_ip(directive);
		serv.listen = directive;
	} 
	else if (name == "root") 
	{
		serv.root = directive;
	} 
	else if (name == "allow") 
	{
		serv.allow = std::vector<Allowed>();
		allow_methods_serv(directive);
	}
	else if (name == "index") 
		serv.index = directive;
	else if (name == "autoindex") 
	{
		if (directive == "on") 
			serv.autoindex = true;
		else if (directive == "off") 
			serv.autoindex = false;
	}
	else if (name == "server_name") 
		serv.server_name = directive;
	else if (name == "location")
		loc.path = directive;
}

void Configurations::take_location_directives(std::string name , std::string directive) 
{
	if (name == "root") 
		loc.root = directive;
	else if (name == "methods") 
	{
		loc.methods = std::vector<Allowed>();
		allow_methods(directive);
	} 
	else if (name == "index") 
		loc.index = directive;
	else if (name == "autoindex") 
	{
		if (directive == "on") 
			loc.autoindex = true;
		else if (directive == "off") 
			loc.autoindex = false;
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
	else if (name == "cgi_extension")
		loc.cgi_extension = directive;
	else if (name == "cgi_path")
		loc.cgi_path = directive;
	else if (name == "client_max_body_size") 
		loc.client_max_body_size = ft_stoi_unsign(directive);
}

void Configurations::config_part() {
	int searching_checker = 0;
	char sym = '{';
	serv_init();
	for(std::vector<std::string>::iterator itc = directives_config.begin(); itc != directives_config.end(); itc++) 
	{
		if (p_config.find(*itc) == 0) {
			if (check_end_line(sym)) {
				server_block = 1;
				searching_checker = 1;
				break;
			}	
		}
	}
}

void Configurations::take_server_part()
{
	loc_init(0);
	int searching_checker = 0;
	char sym = ';';
	if (p_config == "}")
	{ 
		server_block = 0;
		server.push_back(serv);
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
						searching_checker = 1;
					}
				} 
				else if (check_end_line(sym)) 
				{
					take_server_directives(*its, get_directive(*its));
					searching_checker = 1;
					break;
				}	
			}
		}
	}
}

void Configurations::take_location_part()
{
	int searching_checker = 0;
	char sym = ';';
	if (p_config == "}") 
	{
		location_block = 0;
		serv.location.push_back(loc);
	}
	else 
	{
		for(std::vector<std::string>::iterator itl = directives_loc.begin(); itl != directives_loc.end(); itl++) 
		{
			if (p_config.find(*itl) == 0) 
			{
				if (check_end_line(sym)) 
				{
					take_location_directives(*itl, get_directive(*itl));
					searching_checker = 1;
				}	
			}
		}
	}
}

void Configurations::work(std::string file) 
{
	general_init();
	serv_init();
	loc_init(1);
	parser_conf(file);
	while(line_control()) 
	{
		if (get_line()[0] == '#') 
			continue;
		else if (is_server_block() == 0)
			config_part();
		else if (is_server_block() == 1 && is_location_block() == 0)
			take_server_part();
		else if (is_server_block() == 1 && is_location_block() == 1)
			take_location_part();
	}
}

void Configurations::print_parsed() 
{
	std::cout << "__________________\n" << "serv configs num::\t\t" << server.size() << "\n__________________\n\n" << std::endl;
	for(std::vector<Serv_block>::iterator its = server.begin(); its != server.end(); its++) 
	{
		std::cout << "**SERVER BLOCK**" << std::endl;
		std::cout << "__________________\n" << "loc configs num::\t\t" << its->location.size() << "\n__________________\n" << std::endl;
		std::cout << "listen:\t\t" << its->listen << std::endl;
		std::cout << "listen ip:\t\t" << its->ip << std::endl;
		std::cout << "listen port:\t\t" << its->port << std::endl;
		std::cout << "server_name:\t\t" << its->server_name << std::endl;
		std::cout << "root:\t\t" << its->root << std::endl;
		std::cout << "index:\t\t" << its->index << std::endl;
		std::cout << "autoindex:\t\t" << its->autoindex << std::endl;
		if (!(its->allow).empty())
			std::cout << "HERE ARE ALLOWS METHS :" << std::endl;
		// else
		// 	std::cout << "NO ALLOWS METHS" << std::endl;
	
		for(std::vector<Allowed>::iterator k = its->allow.begin(); k != its->allow.end(); k++) 
			std::cout << "server allows >\t\t" << *k << std::endl;

		for(std::vector<Loc_block>::iterator itl = its->location.begin(); itl != its->location.end(); itl++) {
			std::cout << "\n**LOC BLOCK**" << std::endl;
			std::cout << "path:\t\t" << itl->path << std::endl;
			std::cout << "index:\t\t" << itl->index << std::endl;
			std::cout << "autoindex:\t\t" << itl->autoindex << std::endl;
			std::cout << "root:\t\t" << itl->root << std::endl;
			std::cout << "auth_basic:\t\t" << itl->auth_basic << std::endl;
			std::cout << "auth_basic_user_file:\t\t" << itl->auth_basic_user_file << std::endl;
			std::cout << "cgi_extension:\t\t" << itl->cgi_extension << std::endl;
			std::cout << "cgi_path:\t\t" << itl->cgi_path << std::endl;
			std::cout << "client_max_body_size:\t\t" << itl->client_max_body_size << std::endl;
			if (!(itl->methods).empty())
				std::cout << "HERE ARE LOC METHODS :" << std::endl;
			// else
			// 	std::cout << "NO LOC METHODS" << std::endl;

			for(std::vector<Allowed>::iterator j = itl->methods.begin(); j != itl->methods.end(); j++) 
				std::cout << "location's methods >\t\t" << *j << std::endl;
		}
		std::cout << "\n\n" << std::endl;
	}
}