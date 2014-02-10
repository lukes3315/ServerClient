#ifndef __CLIENT
#define __CLIENT

#include <thread>
#include <unistd.h>
#include <iostream>
#include <strings.h>
#include <stdlib.h>

class Server;

class Client
{
public :
  Client(int, int, Server *);
  ~Client();
  void readData(int socketfd);
  bool isRunning()const;
  int getId()const;
private :
  int socketfd;
  int ID;
  bool running;
  Server *server;
  std::thread * backgroundReadingThread;
};

#endif
