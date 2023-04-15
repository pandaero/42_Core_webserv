#include "../include/webserv.hpp"
#include "../include/Server.hpp"

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/resource.h>

int	g_maxPollFds = 0;

void	handleHttpRequest(int sockfd)
{
	char				buffer[1024];
	struct sockaddr_in	client_address;
	socklen_t			client_address_length = sizeof(client_address);
	int					bytesRcvd = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_address, &client_address_length);
	
	if (bytesRcvd == -1)
		std::cerr << "Error: did not receive data from socket." << std::endl;

	std::cout << "Received: " << buffer;

	std::string	received(buffer);
	
	std::string	response = "Received: \"" + received.substr(0, received.size() - 2) + "\" successfully.\n";
	send(sockfd, response.c_str(), response.size(), 0);
}

int	main(int argc, char **argv)
{

	if (argc > 2)
	{
		std::cerr << "Error: too many arguments." << std::endl;
		return (EXIT_FAILURE);
	}

	struct rlimit rlim;
	if (getrlimit(RLIMIT_NOFILE, &rlim) == -1) {
		std::cerr << "Error: could not get maximum number of file descriptors." << std::endl;
		return 1;
	}
	g_maxPollFds = rlim.rlim_cur;

	if (argc == 2) // Parse config file
		(void) argv;
	else // Use default config
		(void) argv;

	std::vector<std::string>	serverParam;
	serverParam.push_back("domain.com");
	serverParam.push_back("ANY");
	serverParam.push_back("9875");
	serverParam.push_back("rroooty");
	serverParam.push_back("1000");
	std::cout << "Port: " << serverParam[PORT] << std::endl;
	Server	server(serverParam);
	std::vector<Server>	servers;
	servers.push_back(server);

	while (true)
	{
		for (size_t i = 0; i < servers.size(); ++i)
		{
			try
			{
				servers[i].poll();
			}
			catch (std::exception & exc)
			{
				// std::cerr << "Error: could not poll on a socket." << std::endl;
				perror("poll");
				continue;
			}
			servers[i].handleConnections();
		}
	}
	return (EXIT_SUCCESS);
}