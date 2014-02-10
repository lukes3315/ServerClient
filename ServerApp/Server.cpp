#include <errno.h>
#include <strings.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include "Server.hpp"

void Server::writeError(const std::string & method_name, const std::string & error_name)
{
  std::cout << "\e[0;31m[" << method_name << "]\e[m " << error_name << std::endl;
}

Server::Server(int port)
{
  this->port = port;
  this->clientIDS = 0;
  int yes = 1;
  //  setsockopt(socketfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
  if ((socketfd = socket(AF_INET,SOCK_STREAM, 0)) < 0)
    {
      writeError("Server contructor socket", "Error creating socket");
      exit(0);
    }
  bzero(&self, sizeof(self));
  self.sin_family = AF_INET;
  self.sin_port = htons(port);
  self.sin_addr.s_addr = INADDR_ANY;

  
  int bind_err;
  if ((bind_err = bind(socketfd, (struct sockaddr*)&self, sizeof(self))) < 0)
    {
      std::cout << "bind error code " << errno << std::endl;
      writeError("Server constructor bind", "Error binding");
      exit(0);
    }
  if (listen(socketfd, 5) != 0)
    {
      writeError("Server constructor listen", "Error listening");
      exit(0);
    }
}

Server::~Server()
{
  if (socket > 0)
    {
      close(socketfd);
    }
  for (int i = 0 ; i < clients.size() ; ++i)
    {
      delete clients[i];
    }
}

int Server::getCurrentConnections()const
{
  return clients.size();
}

bool Server::acceptConnections()
{
  int clientfd;
  struct sockaddr_in client_addr;
  int addrlen = sizeof(client_addr);

  std::cout << "Clienfd = " << clientfd << std::endl;
  clientfd = accept(socketfd, (struct sockaddr*) &client_addr, &addrlen);
  if (clientfd < 0)
    {
      writeError("Server acceptConnection accept", "Error accept");
      return false;
    }
  ++clientIDS;
  Client* c = new Client(clientIDS, clientfd, this);
  std::cout << "Adding client =  = " << clientfd << std::endl;
  clients.push_back(c);
  return true;
}

void Server::stopClient(int sockfd, int id)
{
  std::vector<Client*>::iterator it = clients.begin();
  int i = 0;

  std::cout << "Stopclient number = "<< id <<std::endl;
  for (; it != clients.end() ; ++it)
    {
      if ((*it)->getId() == id)
	{
	  it = clients.erase(it);
	}
    }
}
