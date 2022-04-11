/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf_tools.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alkanaev <alkanaev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/03 18:12:24 by alkanaev          #+#    #+#             */
/*   Updated: 2022/04/03 18:19:56 by alkanaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

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

unsigned int Configurations::ft_stoi_unsign(std::string str) 
{
	unsigned int res = 0;
	size_t len = str.length();
	for(size_t i = 0; i < len; i++) {
		//if (i != 0) {
		res *= 10;
		//}

		res += str[i] - '0';
	}
	return res;
}