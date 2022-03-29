/**
 * @author      : abaudot (aimebaudot@gmail.com)
 * @file        : main
 * @created     : Monday Mar 28, 2022 19:08:36 CEST
 */

# include <iostream>
# include "Events.hpp"
# include "Config.hpp"


int main ( int ac, char ** av )
{
	/* configuration */
	std::vector<ServerBlock*> _servers;
	try {
		Configurations config(ac, av); /* lauch parsing */
		_servers = config.get_server();
	} catch (std::exception &e) {
		std::cerr << "Fatal: " << e.what() << "\n";
		return (1);
	}
	/* utility */
	struct timespec timeout;
	timeout.tv_sec = 1;
	timeout.tv_nsec = 0;
	std::cout << "\n--~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~--\n";
	std::cout << "         🪐    WEEBSERV   🚀           \n";
	std::cout << "--~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~--\n\n";
	/* Event handler */
	Events events(&timeout);
	try {
		events.init(_servers);
		events.run(); /* server loop in there */
	} catch (std::exception &e) {
		std::cerr << "fatal: " << e.what() << "\n";
		return (1);
	}
    return (0); /* bravo */
}

