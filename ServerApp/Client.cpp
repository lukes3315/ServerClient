#include "Client.hpp"

Client::Client(int _id, int _socket, Server *serv)
{
  this->ID = _id;
  running = true;
  this->socketfd = _socket;
  server = serv;
  try
    {
      this->backgroundReadingThread = new std::thread(&Client::readData, 
						      this,
						      _socket);
    }
  catch (std::exception & e)
    {
      std::cout << "Error = " << e.what()<<std::endl;
    }
}

Client::~Client()
{
  running = false;
  if (socketfd > 0)
    {
      close(socketfd);
    }
}

void Client::readData(int _socket)
{  
  std::cout << "Starting thread for client # " << ID << std::endl;
  char * buffer = (char*)malloc(sizeof(char) * 256);
  if (buffer == NULL)
    {
      std::cout << "Malloc error" << std::endl;
    }
  try
    {
      int bytesRead = 1;
      while (running && bytesRead > 0)
	{
	  bytesRead = read(_socket, buffer, 256);
	  
	  std::cout << "Data got : " << buffer << "Bytes read="
		    << bytesRead << std::endl;
	}
    }
  catch (std::exception e)
    {
      
    }
  bzero(buffer, 256);
  free(buffer);
  running = false;
  server->stopClient(socketfd, ID);
  std::cout << "Client #" << this->ID << " closed "<<std::endl;
}

bool Client::isRunning()const
{
  return running;
}

int Client::getId()const
{
  return this->ID;
}
