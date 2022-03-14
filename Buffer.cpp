/**
 * @author      : abaudot (aimebaudot@gmail.com)
 * @file        : Buffer
 * @created     : Thursday Mar 03, 2022 18:16:48 CET
 */

#include "Buffer.hpp"

void	Buffer::resetBuffer(void) {
	first = 0;
	last = 0;
}

bool	Buffer::empty(void) {
	return (last == 0);
}

bool Buffer::recv(int const fd) {
	last = ::recv(fd, b, BUFFSZ, 0);
	return (last > 0);
}

int	Buffer::findChar(std::vector<char>&str, char const c) {
	if (first == last)
		return (0);
	char *finder = std::find(b + first, b + last, c);
	if (finder != b + last)
	{
		str.insert(str.end(), b  + first, finder);
		first = (finder - b) + 1;
		if (first == last)
			resetBuffer();
		return (1);
	}
	str.insert(str.end(), b + first, b + last);
	resetBuffer();
	return (0);
}

