/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wmardin <wmardin@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/07 17:49:49 by pandalaf          #+#    #+#             */
/*   Updated: 2023/04/16 20:12:36 by wmardin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server():
	_name("unnamedServer"),
	_GET(false),
	_POST(false),
	_DELETE(false),
	_dirListing(false),
	_clientMaxBody(10000),
	_backlog(100),
	_maxConns(100),
	_numConns(1)
{
	(void)_numConns;
	setHost("0");
	setPort("3000");
	setRoot("/default/root");
	setDir("/default/dir");
	setUploadDir("/default/upload");
	setCgiDir("/default/CGI");
	setErrorPage("/default/error");

	_pollStructs = new pollfd[_maxConns];
	_pollStructs[0].fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_pollStructs[0].fd == -1)
		throw	socketCreationFailureException();
	_pollStructs[0].events = POLLIN;
	_pollStructs[0].revents = 0;
	if (fcntl(_pollStructs[0].fd, F_SETFL, O_NONBLOCK) == -1)
		throw	fileDescriptorControlFailureException();
	if (bind(_pollStructs[0].fd, (struct sockaddr *) &_serverAddress, sizeof(_serverAddress)) == -1)
	{
		close(_pollStructs[0].fd);
		throw bindFailureException();
	}
	if (listen(_pollStructs[0].fd, SOMAXCONN) == -1)
	{
		close(_pollStructs[0].fd);
		throw listenFailureException();
	}
}

Server::Server(serverConfig config):
	_numConns(1)
{
	setName(config.serverName);
	setHost(config.host);
	setPort(config.port);
	
	setGet(config.get);
	setPost(config.post);
	setDelete(config.delete_);
	setDirListing(config.dirListing);

	setRoot(config.root);
	setDir(config.dir);
	setUploadDir(config.uploadDir);
	setCgiDir(config.cgiDir);
	setErrorPage(config.errorPage);
	
	setClientMaxBody(config.clientMaxBody);
	setBacklog(config.backlog);
	setMaxConnections(config.maxConnections);

	_pollStructs = new pollfd[_maxConns];
	startListening();
}

Server::~Server()
{
	delete [] _pollStructs;
}

void	Server::startListening()
{
	_pollStructs[0].fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_pollStructs[0].fd == -1)
		throw	socketCreationFailureException();
	_pollStructs[0].events = POLLIN;
	_pollStructs[0].revents = 0;
	if (fcntl(_pollStructs[0].fd, F_SETFL, O_NONBLOCK) == -1)
		throw	fileDescriptorControlFailureException();
	if (bind(_pollStructs[0].fd, (struct sockaddr *) &_serverAddress, sizeof(_serverAddress)) == -1)
	{
		close(_pollStructs[0].fd);
		throw bindFailureException();
	}
	if (listen(_pollStructs[0].fd, SOMAXCONN) == -1)
	{
		close(_pollStructs[0].fd);
		throw listenFailureException();
	}
}

void	Server::poll()
{
	if (::poll(_pollStructs, _clients.size() + 1, -1) == -1)
		throw pollFailureException();
}

void	Server::handleConnections()
{
	if (_pollStructs[0].revents & POLLIN)
	{
		Client newClient(_pollStructs[0].fd);
		if (_clients.size() <= (size_t) _maxConns)
			_clients.push_back(newClient);
		else
			throw connectionLimitExceededException();
		_pollStructs[_clients.size()].fd = newClient.getSocketfd();
		_pollStructs[_clients.size()].events = POLLIN;
	}
	if (_clients.size() > 0)
	{
		int i = 0;
		for (std::vector<Client>::iterator clientIt = _clients.begin(); clientIt != _clients.end(); ++clientIt)
		{
			if (_pollStructs[i + 1].revents & POLLIN)
			{
				char buffer[_clientMaxBody];
				size_t	bytesReceived = recv(_pollStructs[i + 1].fd, buffer, RECEIVE_BUFFER, 0);
				if (bytesReceived <= 0)
				{
					close(_pollStructs[i + 1].fd);
					_clients.erase(clientIt);
				}
				else
				{
					std::cout << "Received " << bytesReceived << " bytes from client. Message: " << buffer << "." << std::endl;
					Response	standard;
					standard.setStatusCode(200);
					standard.loadPage("config/index.html");
					standard.buildResponse();
					std::cout << "Sending response." << std::endl;
					int bytesSent = send(_pollStructs[i].fd, standard.send_msg(), standard.send_size(), 0);
					if (bytesSent == -1)
						throw sendFailureException();
				}
				++i;
			}	
			// if (_pollStructs[i + 1].revents & POLLOUT)
			// if (_pollStructs[i + 1].revents & POLLERR)
			// if (_pollStructs[i + 1].revents & POLLHUP)
		}
	}
}

void Server::whoIsI()
{
	std::cout	<< '\n' \
				<< "Name:\t\t" << _name << '\n' \
				<< "Host:\t\t" << _serverAddress.sin_addr.s_addr << '\n' \
				<< "Port:\t\t" << _serverAddress.sin_port << '\n' \
				
				<< "GET:\t\t" << (_GET ? "yes" : "no") << '\n' \
				<< "POST:\t\t" << (_POST ? "yes" : "no") << '\n' \
				<< "DELETE:\t\t" << (_DELETE ? "yes" : "no") << '\n' \
				<< "Dir Listing:\t" << (_dirListing ? "yes" : "no") << '\n' \
				
				<< "Root:\t\t" << _root << '\n' \
				<< "Dir:\t\t" << _dir << '\n' \
				<< "Upload Dir:\t" << _uploadDir << '\n' \
				<< "CGI Dir:\t" << _cgiDir << '\n' \
				<< "Error Page:\t" << _errorPage << '\n' \
				
				<< "Cl. max body:\t" << _clientMaxBody << '\n' \
				<< "Backlog:\t" << _backlog << '\n' \
				<< "Max Conns:\t" << _maxConns << std::endl;
}

const char *	Server::invalidAddressException::what() const throw()
{
	return ("invalid IP address supplied.");
}

const char *	Server::socketCreationFailureException::what() const throw()
{
	return ("error creating socket for server.");
}

const char *	Server::fileDescriptorControlFailureException::what() const throw()
{
	return ("error controlling file descriptor to non-blocking.");
}

const char *	Server::bindFailureException::what() const throw()
{
	return ("error using bind.");
}

const char *	Server::listenFailureException::what() const throw()
{
	return ("error using listen.");
}

const char *	Server::pollFailureException::what() const throw()
{
	return ("error using poll.");
}

const char *	Server::connectionLimitExceededException::what() const throw()
{
	return ("connection limit reached.");
}

const char *	Server::sendFailureException::what() const throw()
{
	return ("error sending data to client.");
}

// SETTERS

void Server::setName(std::string input)
{
	if (!isAlnumString(input))
		throw std::invalid_argument(E_SERVERNAME);
	_name = input;
}

void Server::setHost(std::string input)
{	
	if (input == "ANY")
		_serverAddress.sin_addr.s_addr = INADDR_ANY;
	else
	{
		if (input.find_first_not_of("0123456789.") != std::string::npos)
			throw std::invalid_argument(E_HOSTADDRINPUT);
		_serverAddress.sin_addr.s_addr = inet_addr(input.c_str());
		if (_serverAddress.sin_addr.s_addr == INADDR_NONE)
			throw std::invalid_argument(E_HOSTADDRVAL);
	}
}

void Server::setPort(std::string input)
{
	if (input.find_first_not_of("0123456789") != std::string::npos)
			throw std::invalid_argument(E_PORTINPUT);
	size_t temp = atoi(input.c_str());
	if (temp > 65534)
		throw std::invalid_argument(E_PORTVAL);
	_serverAddress.sin_port = htons(temp);
}

void Server::setGet(bool input)
{
	_GET = input;
}

void Server::setPost(bool input)
{
	_POST = input;
}

void Server::setDelete(bool input)
{
	_DELETE = input;
}

void Server::setDirListing(bool input)
{
	_dirListing = input;
}

void Server:: setRoot(std::string input)
{
	//also URL class?
	_root = input;
}

void Server::setDir(std::string input)
{
	//Probably use URL class here?
	_dir = input;
}

void Server::setUploadDir(std::string input)
{
	//Probably use URL class here?
	_uploadDir = input;
}

void Server::setCgiDir(std::string input)
{
	//Probably use URL class here?
	_cgiDir = input;
}

void Server::setErrorPage(std::string input)
{
	//Probably use URL class here?
	_errorPage = input;
}

void Server::setClientMaxBody(std::string input)
{
	if (input.find_first_not_of("0123456789") != std::string::npos)
			throw std::invalid_argument(E_CMAXBODYINPUT);
	_clientMaxBody = atoi(input.c_str());
	if (_clientMaxBody > CLIENTMAXBODYVAL)
		throw std::invalid_argument(E_CMAXBODYVAL);
}

void Server::setMaxConnections(std::string input)
{
	if (input.find_first_not_of("0123456789") != std::string::npos)
			throw std::invalid_argument(E_MAXCONNINPUT);
	_maxConns = atoi(input.c_str());
	if (_maxConns > MAXCONNECTIONSVAL)
		throw std::invalid_argument(E_MAXCONNVAL);
}

void Server::setBacklog(std::string input)
{
	if (input.find_first_not_of("0123456789") != std::string::npos)
			throw std::invalid_argument(E_BACKLOGINPUT);
	_backlog = atoi(input.c_str());
	if (_backlog > BACKLOGVAL)
		throw std::invalid_argument(E_BACKLOGVAL);
}
