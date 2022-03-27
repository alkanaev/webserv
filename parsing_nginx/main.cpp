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
		exit(1); // temporary decision cause "exit" isn't allowed
	}
	config.work(file);
	if (!config.error_found())
		config.print_parsed();
		// std::cout << config;

}
