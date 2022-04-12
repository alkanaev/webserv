#include "Config.hpp"
#include <stdio.h>

static void close_it(std::string message)
{
	std::cout << message << std::endl;
	exit(1);
}

static bool emptyfile(std::ifstream& file)
{
    return file.peek() == std::ifstream::traits_type::eof();
}

int main(int ac, char **av)
{
	Configurations config;
	
	// config file def
	std::string file;
	if(ac == 2)
	{
		file = av[1];
		std::ifstream fin(file);
		std::ifstream filename(file);

		if(!fin.is_open())
			config.err_message("\n**The configuration file is not valid**\n");
		if(fin.fail() || emptyfile(filename))
			config.err_message("\n**The configuration file is not valid**\n");
	}
	else
		close_it("\n**Please, give a path to a config as 2nd argument**\n");
	config.work(file);
	// if (!config.error_found())
	// 	config.print_parsed();
	// 	config.print_parsed();
#ifdef DEBUG
	print_parsed();
#endif
		// std::cout << config;

}
