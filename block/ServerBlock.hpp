/**
 * @author      : abaudot (aimebaudot@gmail.com)
 * @file        : ServerBlock
 * @created     : Monday Mar 28, 2022 13:36:42 CEST
 */

#ifndef SERVERBLOCK_HPP

#define SERVERBLOCK_HPP


class ServerBlock: protected Serv_block
{
	public:
		explicit ServerBlock ( Serv_block &block ):
			Serv_block(block){}

		virtual ~ServerBlock ();

		/* getter */
		std::string const &get_name() const { return (_name); }
		std::string const &get_ip() const { return (_host); }
		std::string const &get_root() const { return (_root); }
		int get_port() const { return (_port); }
};

#endif /* end of include guard SERVERBLOCK_HPP */

