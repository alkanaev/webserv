/**
 * @author      : abaudot (aimebaudot@gmail.com)
 * @file        : server
 * @created     : Thursday Mar 03, 2022 15:58:03 CET
 */

#include "server.hpp"
#include "Buffer.hpp"

# define RESPONSE "HTTP/1.1 200 OK\r\n\r\n Hello Client !"
# define REPONSE_LEN 24
# define SERVER_PORT 18000

bool	recvLine(int const clientFd, std::string &request)
{
	static Buffer buff;
	static std::vector<char> res;
	
	if (!buff.empty())
		if (buff.findChar(res, '\n'))
		{
			request = std::string(res.begin(), res.end());
			res.clear();
			return (true);
		}
	while (buff.recv(clientFd))
		if (buff.findChar(res, '\n'))
		{
			request = std::string(res.begin(), res.end());
			res.clear();
			return (true);
		}
	request += std::string(res.begin(), res.end());
	res.clear();
	return (false);
}

int	_error(std::string const &msg)
{
	std::cerr << RED << "Error: " << EOCC << msg << "\n";
	return (-1);
}

int main(int ac, char **av)
{
	//useless at this point
	(void)ac; (void)av;
	
	int	listenFd;
	int	clientFd = {0};

	struct sockaddr_in	servAddr;

	/* Create a new socket: socket(int domain, int type, int protocol);
	 * @donain: AF_INET: internet v4 address familly
	 * @type: SOCK_STREAM: TCP Stream
	 * @protocol: 0: Default protocol (TCP)
	 */
	listenFd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenFd < 0)
		return (_error("Socket Creation"));
	
	/* We need to bind the newly cretated socket 
	 * to an IP address and a port number =>
	 * bind(int socket, const struct sockaddr *address, socklen_t address_len);
	 * note: htons convert's values between host and network byte order
	 */
	bzero(&servAddr, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); /*all interfaces available*/
	servAddr.sin_port = htons(SERVER_PORT); /*server port*/

	int rc = bind(listenFd, (struct sockaddr *) &servAddr, sizeof(servAddr));
	if (rc < 0)
		return (_error("Socket Binding"));
	
	/* Listen on connection on the socket: listen(int socket, int backlog);
	 *@backlog: maximum length for the queue of pending connections
	 */
	rc = listen(listenFd, 5);
	if (rc < 0)
		return (_error("Socket listen"));

	/* "Infinit" loop for persistance */
	while (true)
	{
		struct sockaddr clienAddr;
		socklen_t		clienAddrLen;

		std::cout << "Waiting for connection on port: "
			<< BLUE << SERVER_PORT << EOC;
		/* accept(int socket, struct sockaddr *restrict address,
		 * 		socklen_t *restrict address_len);
		 * accept() block the thread until a connection is present.
		 * accept() extracts the first connection request on the queue
		 * of pending connections, creates a new socket with the same
		 * properties of @socket, and allocates a new file descriptor for the socke
		 * @address : get the client address
		 * @address_len : get the client adress lenght
		 */
		clientFd = accept(listenFd, &clienAddr, &clienAddrLen);
		if (clientFd < 0)
			return (_error("Socket accept"));

		{ /* for fun will display the client ip */
			struct sockaddr_in *client = (struct sockaddr_in*)(&clienAddr);
			std::cout << "New client ip Address: " << BLUE << 
				inet_ntoa(client->sin_addr) << EOC;
		}
		
		/* showing the request (not parsing at this point) */
		{
			std::string request;
			while (recvLine(clientFd, request)) /* raw way of doing it */
			{
				std::cout << request << "\n";
				if (request.size() < 2)
					break ;
			}
		}
		std::cout << CYAN << "--------------------------------------" << EOC;

		/* sending default RESPONSE to client */
		rc = send(clientFd, RESPONSE, strlen(RESPONSE), 0);
		if (rc < 0)
			return (_error("Send response"));
		close(clientFd);
	}
	return (0);
}
