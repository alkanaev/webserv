/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf_print.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alkanaev <alkanaev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/03 18:16:39 by alkanaev          #+#    #+#             */
/*   Updated: 2022/04/03 18:17:30 by alkanaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

//selective ptint just to test the "loc_map". will not delete in case we'll need it.
std::ostream &operator<<(std::ostream &ostream_obj, const Loc_block &obj) 
{
	if (obj.root.length() > 0)
		ostream_obj << "root:\t\t" << obj.root << ";" << std::endl;
	if (obj.index.size() > 0) 
	{
		ostream_obj << "index:\t\t";
		std::vector<std::string>::const_iterator it = obj.index.begin();
		for (; it != obj.index.end(); ++it)
			ostream_obj << *it << " ";
		ostream_obj << std::endl;
	}
	return ostream_obj;
}

static void print_LocationBlock(LocationBlock const *block) {
	std::cout << "\n**LOC BLOCK**" << std::endl;
	std::cout << "path:\t\t" << block->get_path() << std::endl;
	std::cout << "index:\t\t";
	for (size_t i = 0; i < block->get_indexs().size(); i++) 
		std::cout << block->get_indexs()[i] << " ";
	std::cout << std::endl;
	std::cout << "autoindex:\t\t" << block->get_autoindex() << std::endl;
	std::cout << "root:\t\t" << block->get_root() << std::endl;
	std::cout << "redirect number:\t\t" << block->get_redirection_code() << std::endl;
	std::cout << "redirect path:\t\t" << block->get_redirection() << std::endl;
	//	std::cout << "auth_basic:\t\t" << block->auth_basic << std::endl;
	//	std::cout << "auth_basic_user_file:\t\t" << itl->auth_basic_user_file << std::endl;
	//	std::cout << "cgi_extension:\t\t" << block->cgi_extension << std::endl;
	//	std::cout << "cgi_path:\t\t" << block->cgi_path << std::endl;
	std::cout << "client_max_body_size:\t\t" << block->get_body_limit() << std::endl;
	std::cout << "UPLOSD_PASS:\t\t" << block->get_upload_pass() << std::endl;
	if (!(block->get_methods()).empty())
		std::cout << "HERE ARE LOC METHODS :" << std::endl;
	std::cout << "location's methods >\t\t[";
	std::cout << block->is_allowed(_GET) << " ";
	std::cout << block->is_allowed(_POST) << " ";
	std::cout << block->is_allowed(_DELETE);
	std::cout << "]" << std::endl;
}

void Configurations::print_parsed() 
{
	std::cout << "__________________\n" << "serv configs num::\t\t" << server.size() << "\n__________________\n\n" << std::endl;
	for(std::vector<ServerBlock*>::const_iterator its = server.begin(); its != server.end(); its++) 
	{
		std::cout << "**SERVER BLOCK**" << std::endl;
		//		std::cout << "__________________\n" << "loc configs num::\t\t" << its->location.size() << "\n__________________\n" << std::endl;
		std::cout << "listen:\t\t" << (*its)->get_listen() << std::endl;
		std::cout << "listen ip:\t\t" << (*its)->get_ip()<< std::endl;
		std::cout << "listen port:\t\t" << (*its)->get_port()<< std::endl;
		std::cout << "server_name:\t\t" << (*its)->get_name()<< std::endl;
		std::cout << "root:\t\t" << (*its)->get_root() << std::endl;
		std::cout << "redirect number:\t\t" << (*its)->get_redirNUm() << std::endl;
		std::cout << "redirect path:\t\t" << (*its)->get_redirection() << std::endl;
		std::cout << "client_max_body_size:\t\t" << (*its)->get_body_limit() << std::endl;
		std::cout << "index:\t\t";
		std::vector<std::string> list = (*its)->get_indexs();
		for (size_t i = 0; i < list.size(); i++) 
			std::cout << list[i] << " ";
		std::cout << std::endl;
		std::cout << "autoindex:\t\t" << (*its)->get_autoindex() << std::endl;
		Serv_block::ErrorObject const &tmp1 = (*its)->get_error_page();
		std::map<int,std::string>::const_iterator itt = tmp1.begin();
		while(itt != tmp1.end())
		{
			std::cout << "error_page\t\t" << itt->first << " :: "<< itt->second << std::endl;
			itt++;
		}
		if (!(*its)->get_allow().empty()) // it can't be empty !
			std::cout << "HERE ARE ALLOWS METHS :" << std::endl;
		std::cout << "server allows >\t\t[";
		std::cout << (*its)->get_allow()[_GET] << " ";
		std::cout << (*its)->get_allow()[_POST] << " ";
		std::cout << (*its)->get_allow()[_DELETE];
		std::cout << "]" << std::endl;

		Serv_block::LocationObject const &tmp = (*its)->get_locations();
		Serv_block::LocationObject::const_iterator it = tmp.begin();
		for (; it != tmp.end(); ++it)
			print_LocationBlock(it->second);
		std::cout << "\n\n" << std::endl;
	}
}
