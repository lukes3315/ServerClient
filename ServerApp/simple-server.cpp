#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <string>
#include <iostream>
#include "Server.hpp"
#include "DisplayManager.hpp"

int main(int ac, char **av)
{ 
  Server * serv = new Server(65111);
  while (1)
    {
      std::cout << "[Before checkClientConnections()" << std::endl;
      serv->checkClientConnections();
      std::cout << "[Before acceptConnections()" << std::endl;
      if (serv->acceptConnections() == true)
	{
	  std::cout << "[After acceptConnections()" << std::endl;
	  std::cout << "Accepted client, current connections = "
		    << serv->getCurrentConnections() << std::endl;
	}
      else
	{
	  sleep(1);
	  std::cout << "Refused client" << std::endl;
	}
    }
  std::cout << "Delete Server" << std::endl;
  delete serv; 
  return 0;
}
