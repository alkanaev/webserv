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

	std::vector<serverBlock *>	_servers;
	_servers.push_back(new serverBlock("127.0.0.2", "mytest.ru", 8080));
	_servers.push_back(new serverBlock("127.0.0.1", "localhost.ru", 8000));

	struct timespec timeout;
	timeout.tv_sec = 1;
	timeout.tv_nsec = 0;
	Events events(&timeout);
	try {
		events.init(_servers);
		events.run();
	} catch (std::exception &e) {
		std::cerr << "fatal: " << e.what() << "\n";
		return (1);
	}
    return 0;
}

