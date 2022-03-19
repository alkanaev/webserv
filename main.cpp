/**
 * @author      : abaudot (aimebaudot@gmail.com)
 * @file        : main
 * @created     : Wednesday Mar 16, 2022 17:54:54 CET
 */

#include <iostream>
# include "Events.hpp"
# include "VHost.hpp"

int main()
{

	/* this part should be get form the parsing */
	/* at this point I whant a vector of server block (ngix config */
	std::vector<serverBlock *>	_servers;
//	_servers.push_back(new serverBlock("192.168.1.2", "mytest.ru", 8000));
	_servers.push_back(new serverBlock("127.0.0.1", "localhost.ru", 8000));
	_servers.push_back(new serverBlock("127.0.0.1", "localhost.ru", 8080));
	_servers.push_back(new serverBlock("127.0.0.1", "localhost.ru", 8010));

	/* utility */
	struct timespec timeout;
	timeout.tv_sec = 1;
	timeout.tv_nsec = 0;

	/* Event handler */
	Events events(&timeout);
	try {
		events.init(_servers);
		events.run(); /* server loop in there */
	} catch (std::exception &e) {
		std::cerr << "fatal: " << e.what() << "\n";
		return (1);
	}
    return 0; /* end */
}

