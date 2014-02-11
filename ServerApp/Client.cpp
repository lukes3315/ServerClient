#include <opencv2/opencv.hpp>
#include "Server.hpp"
#include "Client.hpp"

Client::Client(int _id, int _socket, Server *serv)
{
  this->ID = _id;
  running = true;
  this->socketfd = _socket;
  server = serv;
  try
    {
      std::cout << "Hello how are you ? " << std::endl;
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
  char * buffer = (char*)malloc(sizeof(char) * 1);
  std::string data = "";
  cv::Mat img(480, 640, CV_32FC1);
  if (buffer == NULL)
    {
      std::cout << "Malloc error" << std::endl;
    }
  try
    {
      int bytesRead = 1;
      while (running && bytesRead > 0)
	{
	  bytesRead = read(_socket, buffer, 1);
	  if (data.find("start") != std::string::npos
	      && data.find("end") != std::string::npos)
	    {
	      img.data = data.c_str();
	      data = "";
	    }
	  else
	    {
	      data += buffer;
	    }
	  cv::imshow("win2", img);
	  //std::cout << "Data got : " << buffer << "Bytes read="
	  //	    << bytesRead << std::endl;
	}
    }
  catch (std::exception e)
    {
      
    }
  bzero(buffer, 256);
  free(buffer);
  running = false;
  server->stopClient();
}

bool Client::isRunning()const
{
  return running;
}

int Client::getId()const
{
  return this->ID;
}
