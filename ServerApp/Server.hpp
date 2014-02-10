#ifndef __SERVER
#define __SERVER

#include <vector>
#include "Client.hpp"

class Server
{
  friend class Client;
public :
  Server(int);
  ~Server();
  int getCurrentConnections()const;
  bool acceptConnections();
private :
  std::vector<Client*> clients;
  int socketfd;
  int clientIDS;
  int numberOfClients;  
  int port;
  struct sockaddr_in self;

  void stopClient(int, int);
  void writeError(const std::string&, const std::string&);
};

#endif
