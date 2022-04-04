#include "Config.hpp"

int main(int ac, char **av)
{
	Configurations config;
	
	// config file def
	std::string file;
	if(ac == 2)
		file = av[1];
	else
	{
		std::cout << "\n**Plese, give a path to a config as 2nd argument**\n" << std::endl;
		exit(1);
	}
	config.work(file);
	// if (config.error_found())
	// 	std::
	// 	config.print_parsed();
#ifdef DEBUG
	print_parsed();
#endif
		// std::cout << config;

}
