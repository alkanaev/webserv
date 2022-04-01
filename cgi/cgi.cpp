#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include "Cgi.hpp"

/*
* OK it seems that this is supposed to
* Set many env var
* zxecute some binaries present on the server
* Print a web page which will we displayed on the client
*/

Cgi::Cgi(){}

void Cgi::salut() {
	if(const char* env_var = std::getenv("GATEWAY_INTERFACE"))
		printf("salut, GATEWAY_INTERFACE= [%s]\n", env_var);
}


//(request, root, cgi_path)
std::string Cgi::run_cgi()
{

	std::string content;
	//char const *file_path = "./root/cgi_files/hello";
	char const *file_path = "/bin/ls";
	char const *bin = "-l";
	char const *argv[3];
	
	argv[0] = "ls";
	argv[1] = bin;
	argv[2] = NULL;

	setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
	setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
	setenv("SERVER_PORT", "TODO !", 1);
	setenv("REQUEST_METHOD", "TODO !", 1);
	setenv("SERVER_NAME", "webserv", 1);
	setenv("REDIRECT_STATUS", "1", 1);
	setenv("PATH_INFO", "TODO !", 1);
	setenv("SCRIPT_FILENAME", "TODO", 1);


	extern char * const environ[];
	pid_t   pid;
	int fd[2];

	pid = fork();
	if (!pid) {
		close(fd[0]);
		dup2(fd[1], 1);
		close(fd[1]);


		char * const command[] = {"ls", "-l",  NULL};
		if (execve("/usr/bin/nc", command, environ) == -1) {
			perror("Error: Execve Can't run");
			std::cout << errno << "\n";
		}
	}
	else {
	close(fd[1]);
	int ret = 1;
	char buffer[1024];
	while (ret) {
		memset(buffer, 0, 1024);
		ret = read(fd[0], buffer, 1024);
		content += buffer;
		std::cout << content;
	}
	close(fd[0]);
	waitpid(pid, nullptr, 0);
	}
    	return (content);
	return "return !";
}



int main() {
	Cgi myCgi;
	std::string ret = myCgi.run_cgi();
	myCgi.salut();
	std::cout << ret;
}


