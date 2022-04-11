/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf_checkers.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alkanaev <alkanaev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/03 18:10:16 by alkanaev          #+#    #+#             */
/*   Updated: 2022/04/03 18:24:36 by alkanaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

void Configurations::err_message(std::string str)
{
	error_check_point = 1;
	std::cout << str << std::endl;
}

bool Configurations::error_found() 
{
	return error_check_point;
}

int Configurations::is_server_block() 
{
	return server_block;
}

int Configurations::is_location_block() 
{
	return location_block;
}

int Configurations::check_string(std::string str, int k) 
{
	if (!k)
	{
		if ((str[0] == '/' || (str[0] == '.' && str[1] == '/')))
			return 1;
	}
	else
	{
		if ((str[0] == '/' || (str[0] == '.' && str[1] == '/') || (str[0] == '*' && str[1] == '.')))
			return 1;
	}
	return 0;
}

int Configurations::check_end_line(char i) 
{
	size_t k = (p_config.length() - 1);
	if (p_config[k] == i) 
		return 1;
	return 0;
}