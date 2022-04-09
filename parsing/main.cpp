#include "Config.hpp"

static void close_it(std::string message)
{
	std::cout << message << std::endl;
	exit(1);
}

int main(int ac, char **av)
{
	Configurations config;
	
	// config file def
	std::string file;
	if(ac == 2)
	{
		file = av[1];
		if (access(file.c_str(), F_OK ) == -1)
			close_it("\n**The configuration file is not valid**\n");
	}
	else
		close_it("\n**Plese, give a path to a config as 2nd argument**\n");
	config.work(file);
	// if (!config.error_found())
	// 	config.print_parsed();
	// 	config.print_parsed();
#ifdef DEBUG
	print_parsed();
#endif
		// std::cout << config;

}
